#include "StdAfx.h"
#include "PlayerView.h"

#include "Player/Player.h"
#include "Player/Input/PlayerInput.h"
#include "Player/Movement/PlayerMovement.h"

#include <IViewSystem.h>
#include <CryAnimation/ICryAnimation.h>

CPlayerView::CPlayerView() : m_fZoomRatio(1.0f)
{
}

CPlayerView::~CPlayerView()
{
	GetGameObject()->ReleaseView(this);
}

void CPlayerView::PostInit(IGameObject *pGameObject)
{
	m_pPlayer = static_cast<CPlayer *>(pGameObject->QueryExtension("Player"));

	const int requiredEvents[] = { eGFE_BecomeLocalPlayer };
	pGameObject->RegisterExtForEvents(this, requiredEvents, sizeof(requiredEvents) / sizeof(int));
}

void CPlayerView::HandleEvent(const SGameObjectEvent &event)
{
	if (event.event == eGFE_BecomeLocalPlayer)
	{
		// Register for UpdateView callbacks
		GetGameObject()->CaptureView(this);
	}
}

void CPlayerView::UpdateView(SViewParams &viewParams)
{
	IEntity &entity = *GetEntity();

	float maxDistZ = m_pPlayer->GetCVars().m_viewDistanceZ;
	float minDistZ = 5.0f;
	float distY = m_pPlayer->GetCVars().m_viewDistanceY;

	float distZ = minDistZ + (maxDistZ - minDistZ)*m_fZoomRatio;

	viewParams.position = Vec3::CreateLerp(viewParams.GetPositionLast(), entity.GetWorldPos() + Vec3(distY, 0, distZ), 10.0f * viewParams.frameTime);
	//viewParams.rotation = Quat::CreateRotationXYZ(Vec3(DEG2RAD(-80), 0, 0));
	Vec3 lookDir = (entity.GetWorldPos() - viewParams.position).GetNormalizedSafe();
	lookDir.y = 0;
	lookDir.Normalize();
	Quat lookRot = Quat::CreateRotationVDir(lookDir);
	viewParams.rotation = Quat::CreateSlerp(viewParams.GetRotationLast(), lookRot, 10.f*viewParams.frameTime);
	
	// Start with matching view to the entity position
	/*viewParams.position = entity.GetWorldPos();

	// Offset the camera upwards
	viewParams.position.z += m_pPlayer->GetCVars().m_viewDistanceFromPlayer;

	// Create rotation, facing the player
	viewParams.rotation = Quat::CreateRotationX(DEG2RAD(-90));*/
}

void CPlayerView::Zoom(bool bZoomIn)
{
	if (bZoomIn)
		m_fZoomRatio -= 0.1f;
	else
		m_fZoomRatio += 0.1f;

	m_fZoomRatio = CLAMP(m_fZoomRatio, 0.0f, 1.0f);
}
