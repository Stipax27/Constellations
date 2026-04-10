////////////////////////////////////////////////////////////////////////////////
// Filename: queststarcollection.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _QUESTSTARCOLLECTION_H_
#define _QUESTSTARCOLLECTION_H_

//////////////
// INCLUDES //
//////////////
#include <string>
#include <vector>
#include "questclass.h"
#include "../entity.h"
#include "../Transform.h"
#include "../Star.h"
#include "../SphereCollider.h"
#include "../Health.h"
#include "../ParticleEmitter.h"
#include "../Components/Grabbable.h"

#include "../World.h"
#include "../EntityStorage.h"
#include <cmath>
#include <iostream>
#include <algorithm>

#include "../singleton.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: QuestStarCollection
// Квест: Сбор звёзд к центральной звезде с образованием сверхновой
////////////////////////////////////////////////////////////////////////////////

class QuestStarCollection : public QuestClass {
private:
    // Статические ссылки на глобальные системы
    World* s_World;

    // Корневая сущность квеста
    Entity* m_QuestRoot;

    // Центральная звезда (цель)
    Entity* m_CentralStar;

    // Коллекция маленьких звёзд
    vector<Entity*> m_CollectedStars;
    vector<Entity*> m_StarEntities;

    // Параметры квеста
    int m_StarsToCollect;
    int m_StarsCollected;
    float m_CollectionRadius;
    bool m_SupernovaTriggered;
    bool m_QuestActive;

    // Таймеры и эффекты
    float m_SupernovaTimer;
    float m_SupernovaDuration;

    // Компоненты для эффектов
    ParticleEmitter* m_SupernovaEmitter;
    Entity* m_SupernovaEffect;

    // Приватные методы
    void CreateCentralStar();
    void CreateSmallStars();
    void CheckStarCollection();
    void TriggerSupernova();
    void DestroyAllStars();
    void CreateSupernovaEffect();
    void CreateCollectEffect(const point3d& position);
    void CleanupQuest();

public:
    // Конструктор и деструктор
    QuestStarCollection();
    virtual ~QuestStarCollection();

    // Реализация виртуальных методов QuestClass
    virtual void Start() override;
    virtual void Stop() override;
    virtual void Complete() override;
    virtual void Update() override;

    // Дополнительные методы
    void OnStarCollected(Entity* star);
    bool IsQuestActive() const { return m_QuestActive; }
    int GetStarsCollected() const { return m_StarsCollected; }
    int GetStarsToCollect() const { return m_StarsToCollect; }

    // Метод для проверки коллизии
    void CheckStarCollision(Entity* star, Entity* centralStar);

    // Геттеры для системы коллизий
    Entity* GetCentralStar() const { return m_CentralStar; }
    const vector<Entity*>& GetStarEntities() const { return m_StarEntities; }
};

#endif // _QUESTSTARCOLLECTION_H_