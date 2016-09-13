#pragma once

#include "Entities/Helpers/NativeEntityBase.h"
#include "Player/Player.h"

class CPlayer;

////////////////////////////////////////////////////////
// Physicalized bullet shot from weaponry, expires on collision with another object
////////////////////////////////////////////////////////
class CTurret
	: public CGameObjectExtensionHelper<CTurret, CNativeEntityBase>
{
public:
	CTurret() : m_pPlayer(nullptr) {}
	virtual ~CTurret() {}

	// ISimpleExtension	
	virtual void PostInit(IGameObject *pGameObject) override;
	virtual void HandleEvent(const SGameObjectEvent &event) override;
	// ~ISimpleExtension

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; };
	void ResetModel();

protected:
	void Physicalize();

	CPlayer* m_pPlayer;
};