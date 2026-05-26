// GameStates/GameplayState.h
#pragma once
#include "../Engine/ECS_Base/world.h"
#include "../Engine/Window/windowclass.h"
#include "../Engine/UI/Text/UITextSystem.h"
#include "../Engine/Compute/Quests/QuestSystem.h"
#include "../Engine/Compute/Combat/QTESystem.h"
#include "../Engine/Physic/Collision/CollisionManagerClass.h"
#include "../Engine/Mouse/mouseclass.h"
class LevelManagerClass;
class PlayerController;

class GameplayState
{
private:
    LevelManagerClass* m_Manager;
    World* m_World;
    Entity* m_WorldFolder;
    bool m_IsActive;

    // Переносим сюда все переменные из LevelManagerClass
    PlayerController* playerController;
    Entity* m_CurrentBoss;
    Entity* m_BossHealthFill;
    TextLabel* m_BossNumbersText;

    bool m_IsExecutionActive;
    Entity* m_ExecutionUI;
    float m_ExecutionTimer;

    bool m_ShowGameOverMessage;
    bool m_ShowVictoryMessage;
    float m_MessageTimer;
    bool m_IsInBossArena;

    point3d m_BossArenaCenter;
    float m_BossArenaRadius;

    // QTESystem - если есть в LevelManagerClass
    QTESimpleSystem m_QTESystem;
    QTESimple m_CurrentQTE;

    // Модели и анимации
    //SkinnedMesh m_FoxMesh, m_CesiumMesh, m_PunchComboNewMesh, m_TestAnimMesh;
    //Skeleton m_FoxSkeleton, m_CesiumSkeleton, m_PunchComboNewSkeleton, m_TestAnimSkeleton;
    std::vector<AnimationClip> m_FoxAnimations, m_CesiumAnimations, m_PunchComboNewAnimations, m_TestAnimAnimations;
    Entity* m_TestAnimEntity;
    int m_TestAnimCycleIndex;
    bool m_WasToggleAnimationPressed;

    ID3D11Buffer* m_BoneBuffer;

    // Указатели на системы
    CollisionManagerClass* collisionManager;
    QuestManager* questManager;
    MouseClass* mouse;
    WindowClass* window;

public:
    GameplayState(LevelManagerClass* manager);
    ~GameplayState();

    void Enter();   // Инициализация мира
    void Exit();    // Очистка
    void Update();  // Главный цикл
    void Render();  // Рендер

    bool IsActive() const { return m_IsActive; }

    // Методы для доступа из LevelManagerClass
    World* GetWorld() const { return m_World; }
    WindowClass* GetWindow() const { return window; }

private:
    // Переносим все методы создания из LevelManagerClass
    void InitWindow();
    void InitSystems();
    void LoadModels();

    Entity* CreatePlayer(Entity* folder);
    void CreateUI();
    void CreateSpaceBackground(Entity* folder, int quality);
    void CreateAries(Entity* folder);
    void CreateZenithLocation(Entity* folder, int quality);
    void CreateArenaBarrier(Entity* parent, const point3d& center, float radius, int starCount);
    Entity* CreateMinion(point3d position, float startDelay);
    void CreateNebula(Entity* folder, int quality);
    void CreateStarQuestLoc(Entity* folder, int quality);

    void ShowGameOverMessage(const wchar_t* message, const point3d& color);
    void ShowExecutionUI();
    void HideExecutionUI();
    void ExecuteBoss();
    void BossRecovery();
    void UpdateExecutionTimerUI();
    void UpdateTestAnimationToggle();

    void TriggerExecution();
};