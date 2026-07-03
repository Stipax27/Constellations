#include "MazeLinkSystem.h"

#include "../Engine/Render/Beam.h"
#include "../GlobalConfigs.h"

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

			int index = FindStarPair(entity1, entity2);
			if ((worldTransform1.position - worldTransform2.position).magnitude() <= STAR_LINKING_RADIUS) {

				if (index == -1) {
					starPairs.push_back({ entity1, entity2 });

					Entity* link = entityStorage.CreateEntity("MazeLink");
					link->AddComponent<Transform>();

					Beam* beam = link->AddComponent<Beam>();
					beam->size1 = 1;
					beam->size2 = 1;
					beam->point1 = worldTransform1.position;
					beam->point2 = worldTransform2.position;

					mazeLinks.push_back(link);
				}
				else {
					Entity* link = mazeLinks[index];
					Beam* beam = link->GetComponent<Beam>();
					beam->point1 = worldTransform1.position;
					beam->point2 = worldTransform2.position;
				}

			}
			else {

				if (index != -1) {
					Entity* link = mazeLinks[index];
					link->Destroy();

					starPairs.erase(starPairs.begin() + index);
					mazeLinks.erase(mazeLinks.begin() + index);
				}

			}
		}

	}
}


int MazeLinkSystem::FindStarPair(Entity* star1, Entity* star2)
{
	for (int i = 0; i < starPairs.size(); i++) {
		if (starPairs[i].first == star1 && starPairs[i].second == star2) {
			return i;
		}
	}

	return -1;
}