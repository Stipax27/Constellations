#include "timeSystem.h"

using namespace std;



TimeSystem::TimeSystem()
{
}


void TimeSystem::Initialize()
{
}


void TimeSystem::Shutdown()
{
}


void TimeSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.entities;
	for (Entity* entity : entities) {
		if (!IsEntityValid(entity))
			continue;

		entity->localTime += timer::deltaTime * (double)entity->GetTimeScale();
	}
}