#ifndef _QUEST_SYSTEM_H_
#define _QUEST_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "QuestGiver.h"
#include "Transform.h"
#include "Quests/questmanager.h"


class QuestSystem : public System
{
public:
	QuestSystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);
private:
	Entity* player;
	Transform* playerTransform;
	QuestManager* questManager;
};

#endif
