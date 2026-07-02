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

		point3d worldPos1 = GetWorldTransform(entity1).position;

		for (int j = 0; j < size; j++) {
			Entity* entity2 = entities[j];

			if (!IsEntityValid(entity2) || entity2 == entity1)
				continue;

			point3d worldPos2 = GetWorldTransform(entity2).position;

			if ((worldPos1 - worldPos2).magnitude() <= 5000) {
				Beam* beam = entity1->GetComponent<Beam>();
				if (!beam) {
					beam = entity1->AddComponent<Beam>();

					beam->size1 = 1;
					beam->size2 = 1;
				}
				//beam->point1 = worldPos1;
				beam->point2 = worldPos2;
			}
			else {
				entity1->RemoveComponent<Beam>();
			}
		}

	}
}