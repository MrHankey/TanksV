#include "StdAfx.h"
#include "Turret.h"

#include "Game/GameFactory.h"

class CTurretRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		CGameFactory::RegisterGameObject<CTurret>("Turret");

		RegisterCVars();
	}

	void RegisterCVars()
	{
	}

public:

	float m_mass;
};

CTurretRegistrator g_turretRegistrator;

void CTurret::PostInit(IGameObject *pGameObject)
{
	// Set the model
	const int geometrySlot = 0;

	LoadMesh(geometrySlot, g_bulletRegistrator.m_pGeometry->GetString());

	// Now create the physical representation of the entity
	Physicalize();

	// Make sure that bullets are always rendered regardless of distance
	if (auto *pRenderProxy = static_cast<IEntityRenderProxy *>(GetEntity()->GetProxy(ENTITY_PROXY_RENDER)))
	{
		// Ratio is 0 - 255, 255 being 100% visibility
		pRenderProxy->SetViewDistRatio(255);
	}
}

void CTurret::HandleEvent(const SGameObjectEvent &event)
{
}

void CTurret::Physicalize()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_STATIC;

	physParams.mass = g_bulletRegistrator.m_mass;

	GetEntity()->Physicalize(physParams);
}