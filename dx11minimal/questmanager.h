////////////////////////////////////////////////////////////////////////////////
// Filename: questmanager.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _QUEST_MANAGER_H_
#define _QUEST_MANAGER_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include <memory>
#include "questclass.h"

#include "Engine/Lib/isingleton.h"

/////////////
// GLOBALS //
/////////////


////////////////////////////////////////////////////////////////////////////////
// Class name: QuestManager
////////////////////////////////////////////////////////////////////////////////
class QuestManager : public ISingleton
{
public:
	QuestManager();
	QuestManager(const QuestManager&);
	~QuestManager();

	template <typename T, typename... Args>
	T* AddQuest(Args&&... args)
	{
		auto quest = std::make_unique<T>(std::forward<Args>(args)...);
		T* raw_ptr = quest.get();
		quest->Start();
		activeQuests.push_back(move(quest));

		return raw_ptr;
	}

	template <typename T>
	void RemoveQuest()
	{
		auto it = std::find_if(activeQuests.begin(), activeQuests.end(),
			[](const auto& questPtr) {
				return dynamic_cast<T*>(questPtr.get()) != nullptr;
			});

		if (it != activeQuests.end()) {
			(*it)->Stop();
			activeQuests.erase(it);
		}
	}

	void Initialize();
	void Shutdown();

	void UpdateQuests();

private:
	std::vector<std::unique_ptr<QuestClass>> activeQuests;

private:
	void ClearCompletedQuests();
};

#endif