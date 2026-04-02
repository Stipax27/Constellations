////////////////////////////////////////////////////////////////////////////////
// Filename: QuestManager.cpp
////////////////////////////////////////////////////////////////////////////////
#include "questmanager.h"


QuestManager::QuestManager()
{
}


QuestManager::QuestManager(const QuestManager& other)
{
}


QuestManager::~QuestManager()
{
}


void QuestManager::Initialize()
{
	RegisterQuest<QuestStarCollection>("QuestStarCollection");
}


void QuestManager::Shutdown()
{
}


void QuestManager::CreateQuest(const string& questType) {
    auto it = factory.find(questType);
    if (it == factory.end()) {
        // Логируем ошибку
        return;
    }

    QuestClass* rawQuest = it->second();
    rawQuest->Start();
    activeQuests.push_back(unique_ptr<QuestClass>(rawQuest));
}


void QuestManager::UpdateQuests()
{
	for (auto& quest : activeQuests)
	{
		quest->Update();

		if (quest->completed) {
			quest->Complete();
		}
	}

	ClearCompletedQuests();
}


void QuestManager::ClearCompletedQuests()
{
	auto it = std::remove_if(activeQuests.begin(), activeQuests.end(),
		[](const auto& quest) {
			return quest->completed;
		});

	activeQuests.erase(it, activeQuests.end());
}