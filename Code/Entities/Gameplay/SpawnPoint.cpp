#include "StdAfx.h"
#include "SpawnPoint.h"

#include "Game/GameFactory.h"

#include "Player/Player.h"

class CSpawnPointRegistrator
	: public IEntityRegistrator
{
	virtual void Register() override
	{
		CGameFactory::RegisterNativeEntity<CSpawnPoint>("SpawnPoint", "Gameplay");
	}
};

CSpawnPointRegistrator g_spawnerRegistrator;

void CSpawnPoint::ProcessEvent(SEntityEvent &event)
{
    switch (event.event)
    {
        // Physicalize on level start for Launcher
    case ENTITY_EVENT_START_LEVEL:
        // Editor specific, physicalize on reset, property change or transform change
    case ENTITY_EVENT_RESET:
    case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
    case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
        Reset();
        break;
    }
}

void CSpawnPoint::Reset()
{
    LoadMesh(0, "Objects/tanks/tank_heavy_static.cgf");
}

void CSpawnPoint::SpawnEntity(IEntity &otherEntity)
{
	otherEntity.SetWorldTM(GetEntity()->GetWorldTM());
}