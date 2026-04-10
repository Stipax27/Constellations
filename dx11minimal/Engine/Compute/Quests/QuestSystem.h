#ifndef _QUEST_SYSTEM_H_
#define _QUEST_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>

#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"

#include "QuestGiver.h"
#include "questmanager.h"


class QuestSystem : public System
{
public:
	QuestSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
private:
	Entity* player;
	Transform* playerTransform;
	QuestManager* questManager;
};

#endif
