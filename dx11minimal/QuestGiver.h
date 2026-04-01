// QuestGiver.h
#ifndef _QUEST_GIVER_H_
#define _QUEST_GIVER_H_

#include "component.h"
#include <typeindex>
#include <memory>
#include "questclass.h"

struct QuestGiver : Component {
    std::type_index questType = typeid(void);  // Хранит тип квеста
    float radius = 100.0f;

    // Шаблонный метод установки типа квеста
    template<typename T>
    void SetQuestType() {
        static_assert(std::is_base_of<QuestClass, T>::value, "T must be derived from QuestClass");
        questType = typeid(T);
    }

};

#endif