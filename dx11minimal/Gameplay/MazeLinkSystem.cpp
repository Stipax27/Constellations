#include "MazeLinkSystem.h"

using namespace std;



MazeLinkSystem::MazeLinkSystem()
{
}


void MazeLinkSystem::Initialize()
{
}


void MazeLinkSystem::Shutdown()
{
}


void MazeLinkSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetAllEntitiesByName("RotatingStar");
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity1 = entities[i];

		if (!IsEntityValid(entity1))
			return;

		Transform worldTransform1 = GetWorldTransform(entity1);

		for (int j = 0; j < size; j++) {
			Entity* entity2 = entities[i];

			if (!IsEntityValid(entity2) || entity2 == entity1)
				return;

			Transform worldTransform2 = GetWorldTransform(entity2);


		}

	}
}