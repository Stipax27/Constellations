#ifndef _QUEST_GIVER_H_
#define _QUEST_GIVER_H_

#include "../../ECS_Base/component.h"

struct QuestGiver : Component {
    float radius = 10.0f;
    std::string questType = "TypeName";
};

#endif