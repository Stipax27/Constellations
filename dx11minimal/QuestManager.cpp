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
}


void QuestManager::Shutdown()
{
}


void QuestManager::UpdateQuests()
{
	for (auto& quest : activeQuests)
	{
		quest->Update();
	}
}