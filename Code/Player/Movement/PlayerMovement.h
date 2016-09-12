#pragma once

#include "Entities/Helpers/ISimpleExtension.h"
#include "TankTread.h"

class CPlayer;

////////////////////////////////////////////////////////
// Player extension to manage movement
////////////////////////////////////////////////////////
class CPlayerMovement : public CGameObjectExtensionHelper<CPlayerMovement, ISimpleExtension>
{
public:
	CPlayerMovement();
    virtual ~CPlayerMovement();

	//ISimpleExtension
	virtual void PostInit(IGameObject* pGameObject) override;
    virtual void PostUpdate(float frameTime) override;
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	//~ISimpleExtension

	void Physicalize();

	// Gets the requested movement direction based on input data
	Vec3 GetLocalMoveDirection() const;
    void UpdateTreads(float frameTime);
	bool IsOnGround() const { return m_bOnGround; }
	Vec3 GetGroundNormal() const { return m_groundNormal; }

protected:
	// Get the stats from latest physics thread update
	void GetLatestPhysicsStats(IPhysicalEntity &physicalEntity);
	void UpdateMovementRequest(float frameTime, IPhysicalEntity &physicalEntity);

protected:
	CPlayer *m_pPlayer;

	CTankTread* m_pLeftTread;
	CTankTread* m_pRightTread;

	bool m_bOnGround;
	Vec3 m_groundNormal;
    Vec3 m_vecVelocity;
	Quat m_qQueuedRotation;
	QuatT m_qQueuedTurretRotation;
};
