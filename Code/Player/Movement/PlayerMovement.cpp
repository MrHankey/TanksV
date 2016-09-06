#include "StdAfx.h"
#include "PlayerMovement.h"

#include "Player/Player.h"
#include "Player/Input/PlayerInput.h"

CPlayerMovement::CPlayerMovement()
	: m_bOnGround(false)
{
}

void CPlayerMovement::PostInit(IGameObject *pGameObject)
{
	m_pPlayer = static_cast<CPlayer *>(pGameObject->QueryExtension("Player"));

	// Make sure that this extension is updated regularly via the Update function below
	pGameObject->EnableUpdateSlot(this, 0);
}

void CPlayerMovement::Physicalize()
{
    // Physicalize the player as type Living.
    // This physical entity type is specifically implemented for players
    SEntityPhysicalizeParams physParams;
    physParams.type = PE_LIVING;
    physParams.nFlagsOR = pef_monitor_poststep;
    //physParams.nSlot = 0;

    physParams.mass = m_pPlayer->GetCVars().m_mass;

    pe_player_dimensions playerDimensions;

    // Prefer usage of a cylinder instead of capsule
    playerDimensions.bUseCapsule = false;

    // Specify the size of our cylinder
    playerDimensions.sizeCollider = Vec3(2.2f, 2.2f, 1.2f);

    // Keep pivot at the player's feet (defined in player geometry) 
    playerDimensions.heightPivot = 0.f;
    // Offset collider upwards
    playerDimensions.heightCollider = 2.5f;
    playerDimensions.groundContactEps = 0.004f;

    physParams.pPlayerDimensions = &playerDimensions;

    pe_player_dynamics playerDynamics;
    playerDynamics.kAirControl = 0.f;
    playerDynamics.bActive = 1;
    playerDynamics.gravity = Vec3(0.0f,0.0f,-9.81f);
    playerDynamics.mass = physParams.mass;

    physParams.pPlayerDynamics = &playerDynamics;

    GetEntity()->Physicalize(physParams);
}

void CPlayerMovement::Update(SEntityUpdateContext &ctx, int updateSlot)
{
	IEntity &entity = *GetEntity();
	IPhysicalEntity *pPhysicalEntity = entity.GetPhysics();
	if(pPhysicalEntity == nullptr)
		return;

	// Obtain stats from the living entity implementation
	GetLatestPhysicsStats(*pPhysicalEntity);

	// Send latest input data to physics indicating desired movement direction
	UpdateMovementRequest(ctx.fFrameTime, *pPhysicalEntity);    
}

void CPlayerMovement::GetLatestPhysicsStats(IPhysicalEntity &physicalEntity)
{
	pe_status_living livingStatus;
	if(physicalEntity.GetStatus(&livingStatus) != 0)
	{
		m_bOnGround = !livingStatus.bFlying;

		// Store the ground normal in case it is needed
		// Note that users have to check if we're on ground before using, is considered invalid in air.
		m_groundNormal = livingStatus.groundSlope;
	}
}

void CPlayerMovement::UpdateMovementRequest(float frameTime, IPhysicalEntity &physicalEntity)
{
	if(m_bOnGround)
	{
        pe_action_move moveAction;

        // Apply movement request directly to velocity
        moveAction.iJump = 2;

        const float moveSpeed = m_pPlayer->GetCVars().m_moveSpeed;
        moveAction.dir = GetLocalMoveDirection() * moveSpeed * frameTime;

        // Dispatch the movement request
        physicalEntity.Action(&moveAction);
	}
}

Vec3 CPlayerMovement::GetLocalMoveDirection() const
{
	Vec3 moveDirection = ZERO;

	uint32 inputFlags = m_pPlayer->GetInput()->GetInputFlags();

	if (inputFlags & CPlayerInput::eInputFlag_MoveLeft)
	{
		moveDirection.x -= 1;
	}
	if (inputFlags & CPlayerInput::eInputFlag_MoveRight)
	{
		moveDirection.x += 1;
	}
	if (inputFlags & CPlayerInput::eInputFlag_MoveForward)
	{
		moveDirection.y += 1;
	}
	if (inputFlags & CPlayerInput::eInputFlag_MoveBack)
	{
		moveDirection.y -= 1;
	}

	return moveDirection;
}

void UpdateTreads(float frameTime, Vec3 velocity)
{
    /*m_treads[0].Update();
    m_treads[1].Update();

    var maxTurnReductionSpeed = GameCVars.tank_maxTurnReductionSpeed;
    var turnMult = GameCVars.tank_treadTurnMult;
    var speed = velocity.Length;

    bool hardcore = false;
    if (hardcore)
    {
        m_treads[0].SetThrottle(0.0f);
        m_treads[1].SetThrottle(0.0f);

        if (Input.HasFlag(InputFlags.MoveForward))
        {
            m_treads[1].SetThrottle(1.0f);
        }
        if (Input.HasFlag(InputFlags.MoveLeft))
        {
            m_treads[1].SetThrottle(-1.0f);
        }
        if (Input.HasFlag(InputFlags.MoveBack))
        {
            m_treads[0].SetThrottle(1.0f);
        }
        if (Input.HasFlag(InputFlags.MoveRight))
        {
            m_treads[0].SetThrottle(-1.0f);
        }
    }
    else
    {
        if (Input.HasFlag(InputFlags.MoveForward))
        {

            m_treads[0].SetThrottle(1.0f);
            m_treads[1].SetThrottle(1.0f);

            if (Input.HasFlag(InputFlags.MoveLeft))
            {
                m_treads[0].SetThrottle(1.0f);
                m_treads[1].SetThrottle(MathHelpers.Clamp(turnMult * (speed / maxTurnReductionSpeed), 0, turnMult));
            }
            if (Input.HasFlag(InputFlags.MoveRight))
            {
                m_treads[1].SetThrottle(1.0f);
                m_treads[0].SetThrottle(MathHelpers.Clamp(turnMult * (speed / maxTurnReductionSpeed), 0, turnMult));
            }
        }
        else if (Input.HasFlag(InputFlags.MoveBack))
        {
            m_treads[0].SetThrottle(-1.0f);
            m_treads[1].SetThrottle(-1.0f);

            if (Input.HasFlag(InputFlags.MoveLeft))
            {
                m_treads[0].SetThrottle(-1.0f);
                m_treads[1].SetThrottle(MathHelpers.Clamp(-turnMult * (speed / maxTurnReductionSpeed), -turnMult, 0));
            }
            if (Input.HasFlag(InputFlags.MoveRight))
            {
                m_treads[1].SetThrottle(-1.0f);
                m_treads[0].SetThrottle(MathHelpers.Clamp(-turnMult * (speed / maxTurnReductionSpeed), -turnMult, 0));
            }
        }
        else if (Input.HasFlag(InputFlags.MoveLeft))
        {
            m_treads[0].SetThrottle(1.0f);
            m_treads[1].SetThrottle(-1.0f);
        }
        else if (Input.HasFlag(InputFlags.MoveRight))
        {
            m_treads[1].SetThrottle(1.0f);
            m_treads[0].SetThrottle(-1.0f);
        }
        else
        {
            m_treads[0].SetThrottle(0.0f);
            m_treads[1].SetThrottle(0.0f);
        }
    }*/
}