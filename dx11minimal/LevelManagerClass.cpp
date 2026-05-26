////////////////////////////////////////////////////////////////////////////////
// Filename: LevelManagerClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "LevelManagerClass.h"

#include "GameState/MainMenuState.h"
#include "GameState/GameplayState.h"
#include "GameState/PauseState.h"
#include "GameState/GameOverState.h"


using namespace std;

LevelManagerClass::LevelManagerClass()
{
    window = 0;
    mouse = 0;
    m_World = 0;

    // Инициализация состояний
    m_MainMenuState = nullptr;
    m_GameplayState = nullptr;
    m_PauseState = nullptr;
    m_GameOverState = nullptr;
    m_VictoryState = nullptr;

    m_CurrentState = GameState::MAIN_MENU;
    m_PreviousState = GameState::MAIN_MENU;

    m_IsExecutionActive = false;
    m_ExecutionUI = nullptr;
    m_ExecutionTimer = 0.0f;
    m_ShowGameOverMessage = false;
    m_ShowVictoryMessage = false;
    m_MessageTimer = 0.0f;
    m_IsInBossArena = false;

    playerController = nullptr;
    collisionManager = nullptr;
    questManager = nullptr;
    m_TestAnimEntity = nullptr;
    m_TestAnimCycleIndex = 0;
    m_WasToggleAnimationPressed = false;
    m_BoneBuffer = nullptr;
    testEnemy = nullptr;
    worldFolder = nullptr;
    m_CurrentBoss = nullptr;
    m_BossHealthFill = nullptr;
    m_BossNumbersText = nullptr;
    m_BossNameText = nullptr;
}

LevelManagerClass::LevelManagerClass(const LevelManagerClass& other)
{
}

LevelManagerClass::~LevelManagerClass()
{
}

void LevelManagerClass::InitWindow()
{
    if (window == 0) {
        window = Singleton::GetInstance<WindowClass>();
    }
}

void LevelManagerClass::ProcessSound(const char* name)
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);
}

bool LevelManagerClass::Initialize()
{
    InitWindow();

    // Создаём мир ОДИН РАЗ для всех состояний
    m_World = Singleton::GetInstance<World>();
    m_World->Initialize();

    // Инициализация базовых систем
    collisionManager = Singleton::GetInstance<CollisionManagerClass>();
    collisionManager->Initialize(*m_World->entityStorage);

    mouse = Singleton::GetInstance<MouseClass>();
    mouse->Initialize();

    questManager = Singleton::GetInstance<QuestManager>();
    questManager->Initialize();

    // Инициализация DirectX
    Dx11Init(window->hWnd, window->width, window->height);
    std::thread modelsLoadingThread(&LevelManagerClass::LoadModels, this);

    D3D11_BUFFER_DESC boneDesc = {};
    boneDesc.Usage = D3D11_USAGE_DEFAULT;
    boneDesc.ByteWidth = sizeof(XMMATRIX) * 128;
    boneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&boneDesc, nullptr, &m_BoneBuffer);

    ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));
    ConstBuf::factors.AriesNebulaLerpFactor = 0;
    ConstBuf::UpdateFactors();

    // Загрузка общих текстур
    Textures::LoadDDSTexture("gta", L"..\\dx11minimal\\Resourses\\Textures\\gta.dds");
    Textures::LoadDDSTexture("aperture", L"..\\dx11minimal\\Resourses\\Textures\\aperture.dds");
    Textures::LoadPNGTexture("comicsSpot", L"..\\dx11minimal\\Resourses\\Textures\\comicsSpot.png");

    if (modelsLoadingThread.joinable()) {
        modelsLoadingThread.join();
    }
    else {
        modelsLoadingThread.detach();
    }

    // Создаём все состояния игры
    m_MainMenuState = new MainMenuState(this);
    m_GameplayState = new GameplayState(this);
    m_PauseState = new PauseState(this);
    m_GameOverState = new GameOverState(this);
   

    // Запускаем игру с главного меню
    SwitchToMainMenu();

    return true;
}

void LevelManagerClass::Shutdown()
{
    // Удаляем состояния
    if (m_MainMenuState) { delete m_MainMenuState; m_MainMenuState = nullptr; }
    if (m_GameplayState) { delete m_GameplayState; m_GameplayState = nullptr; }
    if (m_PauseState) { delete m_PauseState; m_PauseState = nullptr; }
    if (m_GameOverState) { delete m_GameOverState; m_GameOverState = nullptr; }
    if (m_VictoryState) { delete m_VictoryState; m_VictoryState = nullptr; }

    if (playerController)
    {
        playerController->Shutdown();
        delete playerController;
        playerController = 0;
    }

    if (m_World)
    {
        m_World->Shutdown();
        delete m_World;
        m_World = 0;
    }

    if (collisionManager)
    {
        collisionManager->Shutdown();
        delete collisionManager;
        collisionManager = 0;
    }

    if (questManager)
    {
        questManager->Shutdown();
        delete questManager;
        questManager = 0;
    }

    if (mouse)
    {
        delete mouse;
        mouse = 0;
    }

    if (window)
    {
        delete window;
        window = 0;
    }

    if (m_BoneBuffer)
    {
        m_BoneBuffer->Release();
        m_BoneBuffer = nullptr;
    }
}

// ===== МЕТОДЫ ПЕРЕКЛЮЧЕНИЯ СОСТОЯНИЙ =====

void LevelManagerClass::SwitchToMainMenu()
{
    // Выходим из всех активных состояний
    if (m_GameplayState && m_GameplayState->IsActive())
        m_GameplayState->Exit();
    if (m_PauseState && m_PauseState->IsActive())
        m_PauseState->Exit();
    if (m_GameOverState && m_GameOverState->IsActive())
        m_GameOverState->Exit();
    
    // Входим в главное меню
    if (m_MainMenuState)
        m_MainMenuState->Enter();

    m_CurrentState = GameState::MAIN_MENU;
}

void LevelManagerClass::SwitchToGameplay()
{
    // Выходим из текущих UI состояний
     if (m_MainMenuState && m_MainMenuState->IsActive())
        m_MainMenuState->Exit();
    if (m_GameOverState && m_GameOverState->IsActive())
        m_GameOverState->Exit();
   
    // Входим в игру
    if (m_GameplayState)
        m_GameplayState->Enter();

    m_CurrentState = GameState::GAMEPLAY;
}

void LevelManagerClass::SwitchToPause()
{
    m_PreviousState = m_CurrentState;
    m_CurrentState = GameState::PAUSED;

    if (m_PauseState)
        m_PauseState->Enter();
}

void LevelManagerClass::ResumeFromPause()
{
    if (m_PauseState)
        m_PauseState->Exit();

    m_CurrentState = m_PreviousState;
}

void LevelManagerClass::SwitchToGameOver()
{
    if (m_GameplayState)
        m_GameplayState->Exit();

    if (m_GameOverState)
        m_GameOverState->Enter();

    m_CurrentState = GameState::GAME_OVER;
}

void LevelManagerClass::SwitchToVictory()
{
    if (m_GameplayState)
        m_GameplayState->Exit();

   

    m_CurrentState = GameState::VICTORY;
}

void LevelManagerClass::RestartGame()
{
    if (m_GameOverState)
        m_GameOverState->Exit();

    if (m_GameplayState)
        m_GameplayState->Enter();

    m_CurrentState = GameState::GAMEPLAY;
}

// ===== НОВЫЙ ПРОСТОЙ FRAME =====
void LevelManagerClass::Frame()
{
    mouse->UpdateSystemCursorVisibility();
    if (!window->IsActive())
        return;

    mouse->Update();
    interp::UpdateTweens();

    // Вызываем Update и Render в зависимости от текущего состояния
    switch (m_CurrentState)
    {
    case GameState::MAIN_MENU:
    {
        if (m_MainMenuState)
        {
            m_MainMenuState->Update();
            m_MainMenuState->Render();
        }
        break;
    }

    case GameState::GAMEPLAY:
    {
        if (m_GameplayState)
        {
            // Проверка на паузу
            if (input::IsKeyPressed(VK_ESCAPE))
            {
                SwitchToPause();
                break;
            }

            m_GameplayState->Update();
            m_GameplayState->Render();
        }
        break;
    }

    case GameState::PAUSED:
    {
        if (m_PauseState)
        {
            // Рендерим игру на фоне
            if (m_GameplayState)
                m_GameplayState->Render();

            // Обновляем меню паузы
            m_PauseState->Update();

            // Рендерим меню паузы поверх игры
            // (рендер происходит внутри Frame через m_World->UpdateRender())
        }
        break;
    }

    case GameState::GAME_OVER:
    {
        if (m_GameOverState)
        {
            m_GameOverState->Update();
            m_GameOverState->Render();
        }
        break;
    }

    case GameState::VICTORY:
    {
        if (m_VictoryState)
        {
            
        }
        break;
    }
    }

    mouse->RenderCursor();
    Draw::Present();
}

// ===== СТАРЫЕ МЕТОДЫ (ОСТАВЛЕНЫ ДЛЯ ОБРАТНОЙ СОВМЕСТИМОСТИ) =====

void LevelManagerClass::InitSystems()
{
    m_World->AddComputeSystem<TimeSystem>();
    m_World->AddComputeSystem<DelayedDestroySystem>();
    m_World->AddComputeSystem<AISystem>();
    m_World->AddComputeSystem<QuestSystem>();
    m_World->AddComputeSystem<RayDamageSystem>();

    m_World->AddPhysicSystem<GravitySystem>();
    m_World->AddPhysicSystem<PhysicSystem>();
    m_World->AddPhysicSystem<CollisionSystem>();
    m_World->AddPhysicSystem<CombatSystem>();
    m_World->AddPhysicSystem<SkeletalAnimationSystem>(context, m_BoneBuffer);

    m_World->AddRenderSystem<MeshSystem>();
    m_World->AddRenderSystem<StarClaySystem>();
    m_World->AddRenderSystem<SkinnedMeshSystem>(m_World->m_Camera->frustum, m_World->m_Camera, m_BoneBuffer);

    if (SHOW_COLLIDERS) {
        m_World->AddRenderSystem<CollisionDrawSystem>();
    }
    if (SHOW_GRAVITY) {
        m_World->AddRenderSystem<GravityDrawSystem>();
    }

    m_World->AddRenderSystem<SpriteSystem>(m_World->m_Camera->frustum, m_BoneBuffer);
    m_World->AddRenderSystem<NebulaSystem>();
    m_World->AddRenderSystem<UISystem>();
    m_World->AddRenderSystem<UITextSystem>();
}

void LevelManagerClass::LoadModels()
{
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\FistBasePos.obj");
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Arrow.obj");
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesBody.obj");
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesArmor.obj");
    Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\Fox.glb", m_FoxMesh, m_FoxSkeleton, m_FoxAnimations);
    Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\CesiumMan.glb", m_CesiumMesh, m_CesiumSkeleton, m_CesiumAnimations);
    Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\PunchComboNew.glb", m_PunchComboNewMesh, m_PunchComboNewSkeleton, m_PunchComboNewAnimations);

    if (Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\TestAnims\\1.glb", m_TestAnimMesh, m_TestAnimSkeleton, m_TestAnimAnimations))
    {
        Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\2.glb", m_TestAnimSkeleton, m_TestAnimAnimations, false);
        Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\3.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
        Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\4.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
    }

    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\SnakeModel.obj");
}

void LevelManagerClass::UpdateTestAnimationToggle()
{
    const bool isTogglePressed = input::IsKeyDown('Y');
    if (!isTogglePressed)
    {
        m_WasToggleAnimationPressed = false;
        return;
    }

    if (m_WasToggleAnimationPressed)
        return;

    m_WasToggleAnimationPressed = true;

    if (!m_TestAnimEntity)
        return;

    SkeletalAnimationComponent* animComp = m_TestAnimEntity->GetComponent<SkeletalAnimationComponent>();
    if (!animComp)
        return;

    const int clipCount = animComp->animationClips ? static_cast<int>(animComp->animationClips->size()) : 0;
    if (clipCount <= 0)
    {
        animComp->ResetToBindPose();
        animComp->animationClip = nullptr;
        animComp->isPlaying = false;
        animComp->currentTime = 0.0f;
        m_TestAnimCycleIndex = 0;
        return;
    }

    m_TestAnimCycleIndex = (m_TestAnimCycleIndex + 1) % (clipCount + 1);
    if (m_TestAnimCycleIndex == 0)
    {
        animComp->ResetToBindPose();
        animComp->animationClip = nullptr;
        animComp->isPlaying = false;
        animComp->currentTime = 0.0f;
        return;
    }

    animComp->SetAnimationByIndex(static_cast<size_t>(m_TestAnimCycleIndex - 1), true);
    animComp->isPlaying = animComp->animationClip != nullptr;
    animComp->isLooping = true;
    animComp->currentTime = 0.0f;
}

Entity* LevelManagerClass::CreatePlayer(Entity* folder)
{
    Entity* player = m_World->entityStorage->CreateEntity("Player", folder);

    Transform* transform = player->AddComponent<Transform>();
    transform->position = point3d(0.0f, 0.0f, 0.0f);

    PhysicBody* physicBody = player->AddComponent<PhysicBody>();
    physicBody->preciseMovement = true;

    SphereCollider* sphereCollider = player->AddComponent<SphereCollider>();
    sphereCollider->radius = 0.75f;
    sphereCollider->collisionGroup = CollisionFilter::Group::Player;

    Health* health = player->AddComponent<Health>();
    health->fraction = Fraction::Player;
    health->destroyOnDeath = false;

    StarClay* starClay = player->AddComponent<StarClay>();
    starClay->blobsRadius = { 0.2f, 0.4f };
    starClay->rate = 20;

    PointCloud* pointCloud = player->AddComponent<PointCloud>();
    pointCloud->index = 0;
    pointCloud->scale = point3d(0.3f, 0.3f, 0.3f);
    pointCloud->pointSize = 0.01f;
    pointCloud->brightness = 0.2f;
    pointCloud->color = point3d(1, 0.6f, 0.9f);

    Entity* grabHitbox = m_World->entityStorage->CreateEntity("GrabHitbox", player);
    transform = grabHitbox->AddComponent<Transform>();
    transform->position = point3d(0, 0, 4);
    sphereCollider = grabHitbox->AddComponent<SphereCollider>();
    sphereCollider->radius = 4;
    sphereCollider->isTouchable = false;

    m_World->entityStorage->SaveEntityToFile(player, "Player");

    return player;
}

void LevelManagerClass::CreateUI()
{
    // Пустая реализация - UI создаётся в GameplayState
}

void LevelManagerClass::CreateSpaceBackground(Entity* folder, int quality)
{
    // Пустая реализация - мир создаётся в GameplayState
}

void LevelManagerClass::CreateAries(Entity* folder)
{
    // Пустая реализация - мир создаётся в GameplayState
}

void LevelManagerClass::CreateZenithLocation(Entity* folder, int quality)
{
    // Пустая реализация - мир создаётся в GameplayState
}

void LevelManagerClass::CreateArenaBarrier(Entity* parent, const point3d& center, float radius, int starCount)
{
    // Пустая реализация - мир создаётся в GameplayState
}

Entity* LevelManagerClass::CreateMinion(point3d position, float startDelay)
{
    // Пустая реализация - мир создаётся в GameplayState
    return nullptr;
}

void LevelManagerClass::CreateNebula(Entity* folder, int quality)
{
    // Пустая реализация - мир создаётся в GameplayState
}

void LevelManagerClass::CreateStarQuestLoc(Entity* folder, int quality)
{
    // Пустая реализация - мир создаётся в GameplayState
}

void LevelManagerClass::ShowGameOverMessage(const wchar_t* message, const point3d& color)
{
    // Пустая реализация - UI создаётся в GameOverState
}

void LevelManagerClass::ShowExecutionUI()
{
    // Пустая реализация - UI создаётся в GameplayState
}

void LevelManagerClass::HideExecutionUI()
{
    // Пустая реализация - UI создаётся в GameplayState
}

void LevelManagerClass::ExecuteBoss()
{
    // Пустая реализация - логика в GameplayState
}

void LevelManagerClass::TriggerExecution()
{
    // Пустая реализация - логика в GameplayState
}

void LevelManagerClass::BossRecovery()
{
    // Пустая реализация - логика в GameplayState
}

void LevelManagerClass::UpdateExecutionTimerUI()
{
    // Пустая реализация - логика в GameplayState
}