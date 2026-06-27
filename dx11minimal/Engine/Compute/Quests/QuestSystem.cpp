#include "QuestSystem.h"
#include "../../Lib/input.h"

using namespace std;



QuestSystem::QuestSystem()
{
}


void QuestSystem::Initialize()
{
	player = Singleton::GetInstance<EntityStorage>()->GetEntityByName("Player");
	playerTransform = player->GetComponent<Transform>();
	questManager = Singleton::GetInstance<QuestManager>();
}


void QuestSystem::Shutdown()
{
}


void QuestSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<QuestGiver>();
	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity))
			continue;

		QuestGiver* questGiver = entity->GetComponent<QuestGiver>();
		if (questGiver != nullptr && questGiver->active) {
			 
			Transform pos1 = GetWorldTransform(player);
			Transform pos2 = GetWorldTransform(entity);
			float dist = (pos2.position - pos1.position).magnitude();

			if (dist <= questGiver->radius && input::IsKeyPressed('R')) {
				
				questManager->CreateQuest(questGiver->questType);
				entity->RemoveComponent<QuestGiver>();

			}

		}
	}
}