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

#include "../singleton.h"

#include "questclass.h"

// QUEST TYPES //
#include "Queststarcollection.h"

using QuestCreator = function<QuestClass* ()>;

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
	void RegisterQuest(const string& name) {
		factory[name] = []() { return new T(); };
	}

	void CreateQuest(const string& questType);

	void Initialize();
	void Shutdown();

	void UpdateQuests();

private:
	vector<unique_ptr<QuestClass>> activeQuests;
	map<string, QuestCreator> factory;

private:
	void ClearCompletedQuests();
};

#endif