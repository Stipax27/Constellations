#include "MazeLinkSystem.h"

#include "../Engine/Render/Beam.h"

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
			continue;

		Transform worldTransform1 = GetWorldTransform(entity1);

		for (int j = 0; j < size; j++) {
			Entity* entity2 = entities[j];

			if (!IsEntityValid(entity2) || entity2 == entity1)
				continue;

			Transform worldTransform2 = GetWorldTransform(entity2);

			if ((worldTransform1.position - worldTransform2.position).magnitude() <= 75) {

				Entity* link = entityStorage.CreateEntity("MazeLink");

				Beam* beam = link->GetComponent<Beam>();
				beam->size1 = 1;
				beam->size2 = 1;
				beam->point1 = worldTransform1.position;
				beam->point2 = worldTransform2.position;
			}
			else {
				entity1->RemoveComponent<Beam>();
			}
		}

	}
}