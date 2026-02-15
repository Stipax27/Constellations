#include "timeSystem.h"



TimeSystem::TimeSystem()
{
}


void TimeSystem::Initialize()
{
}


void TimeSystem::Shutdown()
{
}


void TimeSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	for (Entity* entity : entities) {
		entity->localTime += timer::deltaTime * (double)entity->GetTimeScale();
	}
}