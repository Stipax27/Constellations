////////////////////////////////////////////////////////////////////////////////
// Filename: queststarcollection.cpp
////////////////////////////////////////////////////////////////////////////////

#include "queststarcollection.h"
#include "World.h"
#include "LevelManagerClass.h"
#include "EntityStorage.h"
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

// Инициализация статических переменных
World* QuestStarCollection::s_World = nullptr;
LevelManagerClass* QuestStarCollection::s_LevelManager = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Конструктор
////////////////////////////////////////////////////////////////////////////////
QuestStarCollection::QuestStarCollection()
{
    m_QuestRoot = nullptr;
    m_CentralStar = nullptr;
    m_StarsToCollect = 10;
    m_StarsCollected = 0;
    m_CollectionRadius = 40.0f;
    m_SupernovaTriggered = false;
    m_QuestActive = false;
    m_SupernovaTimer = 0.0f;
    m_SupernovaDuration = 3.0f;
    m_SupernovaEmitter = nullptr;
    m_SupernovaEffect = nullptr;

    name = "Star Collection Quest";
    description = "Collect 10 stars and bring them to the central star to create a supernova!";
    completed = false;
}

////////////////////////////////////////////////////////////////////////////////
// Деструктор
////////////////////////////////////////////////////////////////////////////////
QuestStarCollection::~QuestStarCollection()
{
    CleanupQuest();
}

////////////////////////////////////////////////////////////////////////////////
// Инициализация глобальных ссылок
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::InitializeGlobalReferences(World* world, LevelManagerClass* levelManager)
{
    s_World = world;
    s_LevelManager = levelManager;
}

////////////////////////////////////////////////////////////////////////////////
// Создание центральной звезды
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CreateCentralStar()
{
    if (!s_World || !s_World->entityStorage || !m_QuestRoot) return;

    m_CentralStar = s_World->entityStorage->CreateEntity("CentralStar", m_QuestRoot);
    Transform* transform = m_CentralStar->AddComponent<Transform>();
    transform->position = point3d(0, 0, 0);

    Star* star = m_CentralStar->AddComponent<Star>();
    star->radius = 20.0f;
    star->crownRadius = 25.0f;
    star->color1 = point3d(0.99, 1, 0.51);
    star->color2 = point3d(0.75f, 0.2f, 0.37f);
    star->crownColor = point3d(0.87f, 0.25f, 0.15f);

    SphereCollider* sphereCollider = m_CentralStar->AddComponent<SphereCollider>();
    sphereCollider->radius = 20.0f;
    sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
    sphereCollider->isTouchable = true;

    Health* health = m_CentralStar->AddComponent<Health>();
    health->hp = 100;
    health->maxHp = 100;
    health->fraction = Fraction::Player;

    m_CentralStar->AddComponent<Grabbable>();

    
    Entity* collectionTrigger = s_World->entityStorage->CreateEntity("CollectionTrigger", m_CentralStar);
    Transform* triggerTransform = collectionTrigger->AddComponent<Transform>();
    triggerTransform->position = point3d(0, 0, 0);

    SphereCollider* triggerCollider = collectionTrigger->AddComponent<SphereCollider>();
    triggerCollider->radius = m_CollectionRadius;
    triggerCollider->collisionGroup = CollisionFilter::Group::Projectile;
    triggerCollider->isTouchable = false;
}

////////////////////////////////////////////////////////////////////////////////
// Создание маленьких звёзд
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CreateSmallStars()
{
    if (!s_World || !s_World->entityStorage || !m_QuestRoot) return;

    vector<point3d> starPositions = {
        point3d(200, 200, 0),
        point3d(0, 400, 400),
        point3d(-200, 0, 200),
        point3d(200, 0, -200),
        point3d(-300, -400, 100),
        point3d(400, 300, 0),
        point3d(-400, -500, 0),
        point3d(0, 600, 0),
        point3d(600, -600, 200),
        point3d(-600, 0, 500),
        point3d(0, -100, 300)
    };

    int starsToCreate = min(m_StarsToCollect, (int)starPositions.size());

    for (int i = 0; i < starsToCreate; i++)
    {
        Entity* star = s_World->entityStorage->CreateEntity("CollectableStar", m_QuestRoot);
        Transform* transform = star->AddComponent<Transform>();
        transform->position = starPositions[i];

        Star* starComponent = star->AddComponent<Star>();
        starComponent->radius = 10.0f;
        starComponent->crownRadius = 15.0f;
        starComponent->color1 = point3d(0.79, 0.23, 1);
        starComponent->color2 = point3d(1, 0.91, 0.46);
        starComponent->crownColor = point3d(1, 0.91, 0.46);

        SphereCollider* sphereCollider = star->AddComponent<SphereCollider>();
        sphereCollider->radius = 10.0f;
        sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
        sphereCollider->isTouchable = true;

        Health* health = star->AddComponent<Health>();
        health->hp = 100;
        health->maxHp = 100;
        health->fraction = Fraction::Player;

        star->AddComponent<Grabbable>();

        m_StarEntities.push_back(star);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Создание эффекта сверхновой
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CreateSupernovaEffect()
{
    if (!s_World || !s_World->entityStorage || !m_QuestRoot) return;

    m_SupernovaEffect = s_World->entityStorage->CreateEntity("SupernovaEffect", m_QuestRoot);
    Transform* transform = m_SupernovaEffect->AddComponent<Transform>();
    transform->position = point3d(0, 0, 0);

    m_SupernovaEmitter = m_SupernovaEffect->AddComponent<ParticleEmitter>();
    if (m_SupernovaEmitter)
    {
        m_SupernovaEmitter->vShader = 21;
        m_SupernovaEmitter->gShader = 0;
        m_SupernovaEmitter->pShader = 21;
        m_SupernovaEmitter->size = { 1.0f, 10.0f };
        m_SupernovaEmitter->color = point3d(1.0f, 0.5f, 0.0f);
        m_SupernovaEmitter->opacity = { 1.0f, 0.0f };
        m_SupernovaEmitter->emitDirection = EmitDirection::Front;
        m_SupernovaEmitter->spread = { 360.0f, 360.0f };
        m_SupernovaEmitter->speed = { 50.0f, 100.0f };
        m_SupernovaEmitter->rate = 500.0f;
        m_SupernovaEmitter->lifetime = 2000;
        m_SupernovaEmitter->isHeapEmit = false;
        m_SupernovaEmitter->compress = RenderCompress::none;
        m_SupernovaEmitter->lastEmitTime = timer::currentTime;
    }

    SphereCollider* blastWave = m_SupernovaEffect->AddComponent<SphereCollider>();
    blastWave->radius = 100.0f;
    blastWave->collisionGroup = CollisionFilter::Group::Projectile;
    blastWave->isTouchable = false;
}

////////////////////////////////////////////////////////////////////////////////
// Создание эффекта сбора звезды
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CreateCollectEffect(const point3d& position)
{
    if (!s_World || !s_World->entityStorage || !m_QuestRoot) return;

    Entity* collectEffect = s_World->entityStorage->CreateEntity("CollectEffect", m_QuestRoot);
    Transform* effectTransform = collectEffect->AddComponent<Transform>();
    effectTransform->position = position;

    ParticleEmitter* effectEmitter = collectEffect->AddComponent<ParticleEmitter>();
    if (effectEmitter)
    {
        effectEmitter->vShader = 21;
        effectEmitter->gShader = 0;
        effectEmitter->pShader = 21;
        effectEmitter->size = { 0.5f, 1.5f };
        effectEmitter->color = point3d(1.0f, 0.8f, 0.2f);
        effectEmitter->opacity = { 1.0f, 0.0f };
        effectEmitter->emitDirection = EmitDirection::Front;
        effectEmitter->spread = { 360.0f, 360.0f };
        effectEmitter->speed = { 5.0f, 10.0f };
        effectEmitter->rate = 50.0f;
        effectEmitter->lifetime = 500;
        effectEmitter->isHeapEmit = true;
        effectEmitter->heapEmitRepeats = 1;
        effectEmitter->heapEmitInterval = 100;
        effectEmitter->compress = RenderCompress::none;
        effectEmitter->lastEmitTime = timer::currentTime;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Запуск квеста
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::Start()
{
    if (m_QuestActive) return;

   
    m_QuestActive = true;
    m_StarsCollected = 0;
    m_SupernovaTriggered = false;
    completed = false;

    // Создаём корневую сущность для квеста
    if (s_World && s_World->entityStorage)
    {
        Entity* worldFolder = s_World->entityStorage->GetEntityByName("World");
        if (!worldFolder)
        {
            worldFolder = s_World->entityStorage->CreateEntity("World");
        }
        m_QuestRoot = s_World->entityStorage->CreateEntity("QuestStarCollection", worldFolder);
        Transform* rootTransform = m_QuestRoot->AddComponent<Transform>();
        rootTransform->position = point3d(0, 0, 150);
    }

    // Создаём звёзды
    CreateCentralStar();
    CreateSmallStars();

    cout << "Created " << m_StarEntities.size() << " stars to collect!" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// Остановка квеста
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::Stop()
{
    if (!m_QuestActive) return;

    cout << "Quest Stopped: " << name << endl;
    m_QuestActive = false;
    CleanupQuest();
}

////////////////////////////////////////////////////////////////////////////////
// Завершение квеста
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::Complete()
{
    if (completed) return;

    
    completed = true;
    m_QuestActive = false;

    if (!m_SupernovaTriggered)
    {
        TriggerSupernova();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Обновление квеста
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::Update()
{
    if (!m_QuestActive || completed) return;

    CheckStarCollection();

    if (m_SupernovaTriggered)
    {
        m_SupernovaTimer += 1.0f / 60.0f;

        if (m_SupernovaTimer >= m_SupernovaDuration)
        {
            if (m_SupernovaEmitter)
            {
                m_SupernovaEmitter->rate = 0.0f;
            }

            if (m_SupernovaEffect && s_World && s_World->entityStorage)
            {
                m_SupernovaEffect->Destroy();
                m_SupernovaEffect = nullptr;
                m_SupernovaEmitter = nullptr;
            }
        }
        else
        {
            if (m_SupernovaEffect)
            {
                Transform* transform = m_SupernovaEffect->GetComponent<Transform>();
                if (transform)
                {
                    float scale = 1.0f + (m_SupernovaTimer / m_SupernovaDuration) * 10.0f;
                    transform->scale = point3d(scale, scale, scale);
                }

                if (m_SupernovaEmitter)
                {
                    float progress = m_SupernovaTimer / m_SupernovaDuration;
                    m_SupernovaEmitter->color = point3d(1.0f, 0.5f * (1.0f - progress), 0.0f);
                    m_SupernovaEmitter->size = { 1.0f + progress * 10.0f, 10.0f + progress * 20.0f };
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Проверка сбора звёзд
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CheckStarCollection()
{
    if (m_StarsCollected >= m_StarsToCollect && !m_SupernovaTriggered)
    {
        TriggerSupernova();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Обработка сбора звезды
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::OnStarCollected(Entity* star)
{
    if (!m_QuestActive || completed) return;

    auto it = find(m_CollectedStars.begin(), m_CollectedStars.end(), star);
    if (it != m_CollectedStars.end()) return;

    m_CollectedStars.push_back(star);
    m_StarsCollected++;

    point3d starPos = point3d(0, 0, 0);
    Transform* starTransform = star->GetComponent<Transform>();
    if (starTransform)
    {
        starPos = starTransform->position;
    }

    CreateCollectEffect(starPos);

    if (star && s_World && s_World->entityStorage)
    {
        star->Destroy();
    }

    auto starIt = find(m_StarEntities.begin(), m_StarEntities.end(), star);
    if (starIt != m_StarEntities.end())
    {
        m_StarEntities.erase(starIt);
    }


    if (m_CentralStar)
    {
        Star* centralStarComp = m_CentralStar->GetComponent<Star>();
        if (centralStarComp)
        {
            float progress = m_StarsCollected / (float)m_StarsToCollect;
            float newRadius = 20.0f + progress * 30.0f;
            centralStarComp->radius = newRadius;
            centralStarComp->crownRadius = newRadius + 5.0f;
            centralStarComp->color1 = point3d(
                0.99f - progress * 0.5f,
                1.0f - progress * 0.8f,
                0.51f + progress * 0.49f
            );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Проверка коллизии
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CheckStarCollision(Entity* star, Entity* centralStar)
{
    if (!star || !centralStar || !m_QuestActive || completed) return;
    if (centralStar != m_CentralStar) return;

    auto it = find(m_StarEntities.begin(), m_StarEntities.end(), star);
    if (it == m_StarEntities.end()) return;

    Transform* starTransform = star->GetComponent<Transform>();
    Transform* centralTransform = centralStar->GetComponent<Transform>();

    if (starTransform && centralTransform)
    {
        point3d starPos = starTransform->position;
        point3d centralPos = centralTransform->position;

        float distance = sqrt(pow(starPos.x - centralPos.x, 2) +
            pow(starPos.y - centralPos.y, 2) +
            pow(starPos.z - centralPos.z, 2));

        if (distance <= m_CollectionRadius)
        {
            OnStarCollected(star);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Активация сверхновой
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::TriggerSupernova()
{
    if (m_SupernovaTriggered) return;


    m_SupernovaTriggered = true;
    m_SupernovaTimer = 0.0f;

    CreateSupernovaEffect();
    DestroyAllStars();

    if (m_CentralStar)
    {
        Star* centralStar = m_CentralStar->GetComponent<Star>();
        if (centralStar)
        {
            centralStar->radius = 80.0f;
            centralStar->crownRadius = 100.0f;
            centralStar->color1 = point3d(1, 0.2f, 0.2f);
            centralStar->color2 = point3d(1, 0.5f, 0);
            centralStar->crownColor = point3d(1, 0.8f, 0);
        }

        SphereCollider* centralCollider = m_CentralStar->GetComponent<SphereCollider>();
        if (centralCollider)
        {
            centralCollider->radius = 100.0f;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Уничтожение всех звёзд
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::DestroyAllStars()
{
    for (Entity* star : m_StarEntities)
    {
        if (star && s_World && s_World->entityStorage)
        {
            Transform* starTransform = star->GetComponent<Transform>();
            if (starTransform)
            {
                CreateCollectEffect(starTransform->position);
            }
            star->Destroy();
        }
    }
    m_StarEntities.clear();
    m_CollectedStars.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Очистка ресурсов
////////////////////////////////////////////////////////////////////////////////
void QuestStarCollection::CleanupQuest()
{
    if (m_SupernovaEffect && s_World && s_World->entityStorage)
    {
        m_SupernovaEffect->Destroy();
        m_SupernovaEffect = nullptr;
        m_SupernovaEmitter = nullptr;
    }

    DestroyAllStars();

    if (m_CentralStar && s_World && s_World->entityStorage)
    {
        m_CentralStar->Destroy();
        m_CentralStar = nullptr;
    }

    if (m_QuestRoot && s_World && s_World->entityStorage)
    {
        m_QuestRoot->Destroy();
        m_QuestRoot = nullptr;
    }
}