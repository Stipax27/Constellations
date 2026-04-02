// QuestGiver.h
#ifndef _QUEST_GIVER_H_
#define _QUEST_GIVER_H_

#include "component.h"

struct QuestGiver : Component {
    float radius = 100.0f;
    string questType = "TypeName";
};

#endif