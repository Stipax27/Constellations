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
#include <map>
#include <functional>

#include "questclass.h"

#include "../../Lib/singleton.h"

// QUEST TYPES //
#include "Queststarcollection.h"

using QuestCreator = std::function<QuestClass* ()>;

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

	template<typename T>
	void RegisterQuest(const std::string& name) {
		factory[name] = []() { return new T(); };
	}

	void CreateQuest(const std::string& questType);

	void Initialize();
	void Shutdown();

	void UpdateQuests();

private:
	std::vector<std::unique_ptr<QuestClass>> activeQuests;
	std::map<std::string, QuestCreator> factory;

private:
	void ClearCompletedQuests();
};

#endif