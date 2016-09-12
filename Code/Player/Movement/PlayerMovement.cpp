#include "StdAfx.h"
#include "PlayerMovement.h"

#include "Player/Player.h"
#include "Player/Input/PlayerInput.h"

CPlayerMovement::CPlayerMovement()
	: m_bOnGround(false)
{
	m_pLeftTread = new CTankTread(3000, Vec2(-1.5f, 0));
	m_pRightTread = new CTankTread(3000, Vec2(1.5f, 0));
}

CPlayerMovement::~CPlayerMovement()
{
	delete m_pLeftTread;
	delete m_pRightTread;
}

void CPlayerMovement::PostInit(IGameObject *pGameObject)
{
	m_pPlayer = static_cast<CPlayer *>(pGameObject->QueryExtension("Player"));

	// Make sure that this extension is updated regularly via the Update function below
	pGameObject->EnableUpdateSlot(this, 0);
    pGameObject->EnablePostUpdates(this);
}

void CPlayerMovement::PostUpdate(float frameTime)
{
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

    UpdateTreads(ctx.fFrameTime);

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
        m_vecVelocity = livingStatus.vel;
	}
}

void CPlayerMovement::UpdateMovementRequest(float frameTime, IPhysicalEntity &physicalEntity)
{	
    float mass = 1000.0f;
    float frontalArea = 20.0f;
    float dragCoefficient = 0.8f;
    float groundFriction = 1.0f;
    float momentumInertia = 2000.0f;
    float AirDensity = 1.27f;

    float slopeAngle = m_bOnGround ? (float)atan2(sqrt(m_groundNormal.x * m_groundNormal.y + m_groundNormal.y * m_groundNormal.y), m_groundNormal.z) : DEG2RAD(90);

    float fForceLeft = m_pLeftTread->GetCurrentForce();
    float fForceRight = m_pRightTread->GetCurrentForce();
    float totalForce = fForceLeft + fForceRight;

    //gEnv->pRenderer->Draw

    Quat prevRotation = GetEntity()->GetWorldRotation();

    // ROTATION
    float totalMomentum = fForceLeft*m_pLeftTread->GetLocalPosition().x + fForceRight*m_pRightTread->GetLocalPosition().x;
    float angularAcceleration = totalMomentum / momentumInertia;

    Quat turnRot = Quat::CreateRotationZ(angularAcceleration * frameTime);
    Quat newRotation = turnRot;

    Vec3 slopeProjection = Vec3::CreateProjection(prevRotation.GetInverted().GetColumn1(), m_groundNormal);
    slopeProjection.Normalize();

    //TODO: Fix slope projection
    //Quat newRotation = prevRotation * turnRot * Quat::CreateRotationVDir(slopeProjection);
    
    newRotation = prevRotation * turnRot;// *Quat::CreateRotationVDir(slopeProjection);
    newRotation.Normalize();

    GetEntity()->SetWorldTM(Matrix34::Create(Vec3(1.0f), newRotation, GetEntity()->GetWorldPos()));

    // VELOCITY
    Vec3 forwardDir = prevRotation.GetColumn1().GetNormalized();

    Vec3 normalizedVelocity;
    if (!m_vecVelocity.IsZero())
        normalizedVelocity = m_vecVelocity.GetNormalized();
    else
        normalizedVelocity = forwardDir.GetNormalized();

    float terminalVelocity = (float)sqrt(abs(2 * mass * abs(9.81f) * (sin(slopeAngle) - groundFriction * cos(slopeAngle))) / (AirDensity * dragCoefficient * frontalArea));
    float velocityRatio = m_vecVelocity.GetLength() / terminalVelocity;

    //F = m*a
    float acceleration = (totalForce / mass);
    Vec3 forwardAcceleration = forwardDir * acceleration;// *GameCVars.tank_movementSpeedMult;

                                                        //TODO: Do proper tread-dependant friction and calculation
    Vec3 frictionDeceleration = (normalizedVelocity * velocityRatio) * (float)(groundFriction * abs(9.81f) * cos(slopeAngle));

    Vec3 dragDeceleration = (dragCoefficient * frontalArea * AirDensity * (normalizedVelocity * (float)pow(m_vecVelocity.GetLength(), 2))) / (2 * mass);

    // TURRET ROTATION
    Vec3 cursorPos = m_pPlayer->GetInput()->GetWorldCursorPosition();
    Vec3 dir = ((cursorPos) - m_pPlayer->GetEntity()->GetWorldPos()).GetNormalizedFast();

    IAttachment* pAttachment = m_pPlayer->GetEntity()->GetCharacter(CPlayer::eGeometry_ThirdPerson)->GetIAttachmentManager()->GetInterfaceByName("turret");
    if (pAttachment)
    {
        QuatT oldTransRot(pAttachment->GetAttAbsoluteDefault());
        //Quat deltaRot = Quat::CreateRotationZ(DEG2RAD(90));
        Quat targetRot = Quat::CreateRotationZ(atan2(-dir.x, dir.y) - newRotation.GetRotZ());
        Quat newRot = Quat::CreateSlerp(oldTransRot.q, targetRot, frameTime * 10);
        oldTransRot.q = newRot;
        pAttachment->SetAttAbsoluteDefault(oldTransRot);
    }

    if (true)
    {
        gEnv->pRenderer->Draw2dLabel(100, 80, 1.3f, Col_White, false, "TerminalVel: %f", terminalVelocity);
        gEnv->pRenderer->Draw2dLabel(100.f, 90.f, 1.3f, Col_White, false, "Speed: %f", m_vecVelocity.GetLength());
        gEnv->pRenderer->Draw2dLabel(100, 100, 1.3f, Col_Red, false, "angularAcceleration: %f", angularAcceleration);
        gEnv->pRenderer->Draw2dLabel(100, 120, 1.2f, Col_White, false, "acceleration: %f", forwardAcceleration - frictionDeceleration - dragDeceleration);
        gEnv->pRenderer->Draw2dLabel(100, 130, 1.2f, Col_Red, false, "forceLeft: %f", floor(fForceLeft));
        gEnv->pRenderer->Draw2dLabel(100, 140, 1.2f, Col_Red, false, "forceRight: %f", floor(fForceRight));
        gEnv->pRenderer->Draw2dLabel(100, 150, 1.3f, Col_Green, false, "totalMomentum: %f", floor(totalMomentum));
        gEnv->pRenderer->Draw2dLabel(100, 160, 1.3f, Col_Blue, false, "lThrottle: %f rThrottle: %f", m_pLeftTread->GetThrottle(), m_pRightTread->GetThrottle());
        gEnv->pRenderer->Draw2dLabel(100, 170, 1.3f, Col_White, false, "slopeX: %f slopeY: %f slopeZ: %f", slopeProjection.x, slopeProjection.y, slopeProjection.z);
    }


    pe_action_move moveAction;

    // Apply movement request directly to velocity
    moveAction.iJump = 1;

    //const float moveSpeed = m_pPlayer->GetCVars().m_moveSpeed;
    //moveAction.dir = GetLocalMoveDirection() * moveSpeed * frameTime;

    moveAction.dir = m_vecVelocity + (forwardAcceleration - frictionDeceleration - dragDeceleration);

    // Dispatch the movement request
    physicalEntity.Action(&moveAction);
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

void CPlayerMovement::UpdateTreads(float frameTime)
{
    float maxTurnReductionSpeed = 4.0f;//GameCVars.tank_maxTurnReductionSpeed;
    float turnMult = 0.3f;
    float speed = m_vecVelocity.GetLength();

    uint32 inputFlags = m_pPlayer->GetInput()->GetInputFlags();

    bool hardcore = false;
    if (hardcore)
    {
        m_pLeftTread->SetThrottle(0.0f);
        m_pRightTread->SetThrottle(0.0f);

        if (inputFlags & CPlayerInput::eInputFlag_MoveForward)
        {
            m_pLeftTread->SetThrottle(1.0f);
        }
        if (inputFlags & CPlayerInput::eInputFlag_MoveLeft)
        {
            m_pLeftTread->SetThrottle(-1.0f);
        }
        if (inputFlags & CPlayerInput::eInputFlag_MoveBack)
        {
            m_pRightTread->SetThrottle(1.0f);
        }
        if (inputFlags & CPlayerInput::eInputFlag_MoveRight)
        {
            m_pRightTread->SetThrottle(-1.0f);
        }
    }
    else
    {
        if (inputFlags & CPlayerInput::eInputFlag_MoveForward)
        {

            m_pLeftTread->SetThrottle(1.0f);
            m_pRightTread->SetThrottle(1.0f);


            if (inputFlags & CPlayerInput::eInputFlag_MoveLeft)
            {
                m_pRightTread->SetThrottle(1.0f);
                m_pLeftTread->SetThrottle(CLAMP(turnMult * (speed / maxTurnReductionSpeed), 0, turnMult));
            }
            if (inputFlags & CPlayerInput::eInputFlag_MoveRight)
            {
                m_pLeftTread->SetThrottle(1.0f);
                m_pRightTread->SetThrottle(CLAMP(turnMult * (speed / maxTurnReductionSpeed), 0, turnMult));
            }
        }
        else if (inputFlags & CPlayerInput::eInputFlag_MoveBack)
        {
            m_pLeftTread->SetThrottle(-1.0f);
            m_pRightTread->SetThrottle(-1.0f);

            if (inputFlags & CPlayerInput::eInputFlag_MoveLeft)
            {
                m_pRightTread->SetThrottle(-1.0f);
                m_pLeftTread->SetThrottle(CLAMP(-turnMult * (speed / maxTurnReductionSpeed), -turnMult, 0));
            }
            if (inputFlags & CPlayerInput::eInputFlag_MoveRight)
            {
                m_pLeftTread->SetThrottle(-1.0f);
                m_pRightTread->SetThrottle(CLAMP(-turnMult * (speed / maxTurnReductionSpeed), -turnMult, 0));
            }
        }
        else if (inputFlags & CPlayerInput::eInputFlag_MoveLeft)
        {
            m_pRightTread->SetThrottle(1.0f);
            m_pLeftTread->SetThrottle(-1.0f);
        }
        else if (inputFlags & CPlayerInput::eInputFlag_MoveRight)
        {
            m_pLeftTread->SetThrottle(1.0f);
            m_pRightTread->SetThrottle(-1.0f);
        }
        else
        {
            m_pRightTread->SetThrottle(0.0f);
            m_pLeftTread->SetThrottle(0.0f);
        }
    }
}