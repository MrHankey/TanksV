#pragma once

#include "Entities/Helpers/NativeEntityBase.h"

////////////////////////////////////////////////////////
// Physicalized bullet shot from weaponry, expires on collision with another object
////////////////////////////////////////////////////////
class CTurret
	: public CGameObjectExtensionHelper<CTurret, CNativeEntityBase>
{
public:
	virtual ~CTurret() {}

	// ISimpleExtension	
	virtual void PostInit(IGameObject *pGameObject) override;
	virtual void HandleEvent(const SGameObjectEvent &event) override;
	// ~ISimpleExtension

protected:
	void Physicalize();
};