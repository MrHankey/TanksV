#pragma once

#include "Entities/Helpers/NativeEntityBase.h"

////////////////////////////////////////////////////////
// Entity responsible for spawning other entities
////////////////////////////////////////////////////////
class CSpawnPoint : public CGameObjectExtensionHelper<CSpawnPoint, CNativeEntityBase>
{
public:
	virtual ~CSpawnPoint() {}
    void SpawnEntity(IEntity &otherEntity);

    // ISimpleExtension	
    virtual void ProcessEvent(SEntityEvent &event) override;
    // ~ISimpleExtension

private:
    
    void Reset();
};