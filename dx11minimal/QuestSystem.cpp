#include "QuestSystem.h"



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


void QuestSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];
		if (IsEntityValid(entity)) {
			QuestGiver* questGiver = entity->GetComponent<QuestGiver>();
			if (questGiver != nullptr && questGiver->active) {
			 
				Transform pos1 = GetWorldTransform(player);
				Transform pos2 = GetWorldTransform(entity);
				float dist = (pos2.position - pos1.position).magnitude();

				if (dist<= questGiver->radius) {
				
					questManager->AddQuest<>();

				}

			}

		}
	}
}