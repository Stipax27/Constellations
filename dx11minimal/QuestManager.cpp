////////////////////////////////////////////////////////////////////////////////
// Filename: QuestManager.cpp
////////////////////////////////////////////////////////////////////////////////
#include "questmanager.h"

using namespace std;


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
}


void QuestManager::Shutdown()
{
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