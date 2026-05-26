// GameStates/GameplayState.cpp
#include "GameplayState.h"
#include "..\\LevelManagerClass.h"

GameplayState::GameplayState(LevelManagerClass* manager)
    : m_Manager(manager)
    , m_World(nullptr)
    , m_WorldFolder(nullptr)
    , m_IsActive(false)
    , playerController(nullptr)
    , m_CurrentBoss(nullptr)
    , m_BossHealthFill(nullptr)
    , m_BossNumbersText(nullptr)
    , m_IsExecutionActive(false)
    , m_ExecutionUI(nullptr)
    , m_ExecutionTimer(0.0f)
    , m_ShowGameOverMessage(false)
    , m_ShowVictoryMessage(false)
    , m_MessageTimer(0.0f)
    , m_IsInBossArena(false)
    , m_BossArenaCenter(0, 0, 200)
    , m_BossArenaRadius(60.0f)
    , collisionManager(nullptr)
    , questManager(nullptr)
    , mouse(nullptr)
    , window(nullptr)
    , m_TestAnimEntity(nullptr)
    , m_TestAnimCycleIndex(0)
    , m_WasToggleAnimationPressed(false)
    , m_BoneBuffer(nullptr)
{
}

GameplayState::~GameplayState()
{
    Exit();
}

void GameplayState::InitWindow()
{
    if (window == 0) {
        window = Singleton::GetInstance<WindowClass>();
    }
}

void GameplayState::Enter()
{
    if (m_IsActive) return;

    InitWindow();

    m_World = Singleton::GetInstance<World>();
    m_World->Initialize();

    collisionManager = Singleton::GetInstance<CollisionManagerClass>();
    collisionManager->Initialize(*m_World->entityStorage);

    mouse = Singleton::GetInstance<MouseClass>();
    mouse->Initialize();

    questManager = Singleton::GetInstance<QuestManager>();
    questManager->Initialize();

    Dx11Init(window->hWnd, window->width, window->height);
    std::thread modelsLoadingThread(&GameplayState::LoadModels, this);

    D3D11_BUFFER_DESC boneDesc = {};
    boneDesc.Usage = D3D11_USAGE_DEFAULT;
    boneDesc.ByteWidth = sizeof(XMMATRIX) * 128;
    boneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&boneDesc, nullptr, &m_BoneBuffer);

    ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));
    ConstBuf::factors.AriesNebulaLerpFactor = 0;
    ConstBuf::UpdateFactors();

    Textures::LoadDDSTexture("gta", L"..\\dx11minimal\\Resourses\\Textures\\gta.dds");
    Textures::LoadDDSTexture("aperture", L"..\\dx11minimal\\Resourses\\Textures\\aperture.dds");
    Textures::LoadPNGTexture("comicsSpot", L"..\\dx11minimal\\Resourses\\Textures\\comicsSpot.png");

    if (modelsLoadingThread.joinable()) {
        modelsLoadingThread.join();
    }
    else {
        modelsLoadingThread.detach();
    }

    m_WorldFolder = m_World->entityStorage->CreateEntity("World");
    Entity* player = CreatePlayer(m_WorldFolder);

    CreateSpaceBackground(m_WorldFolder, 1);
    CreateAries(m_WorldFolder);
    CreateZenithLocation(m_WorldFolder, 2);
    CreateUI();

    InitSystems();

    playerController = new PlayerController();
    playerController->Initialize(player);

    // Создание тестового скелетного персонажа
    auto spawnSkinned = [this](
        const char* name,
        const point3d& pos,
        const point3d& scale,
        const SkinnedMesh& srcMesh,
        Skeleton* skeleton,
        std::vector<AnimationClip>* animations,
        const char* preferredClip = nullptr,
        bool autoPlay = true) -> Entity*
        {
            Entity* e = m_World->entityStorage->CreateEntity(name, m_WorldFolder);
            Transform* t = e->AddComponent<Transform>();
            t->position = pos;
            t->scale = scale;

            SkinnedMesh* meshComp = e->AddComponent<SkinnedMesh>();
            meshComp->vertices = srcMesh.vertices;
            meshComp->indices = srcMesh.indices;
            meshComp->gpuModelIndex = srcMesh.gpuModelIndex;
            meshComp->active = false;

            PointCloud* pointCloud = e->AddComponent<PointCloud>();
            pointCloud->index = srcMesh.gpuModelIndex;
            pointCloud->vShader = 28;
            pointCloud->gShader = 17;
            pointCloud->pShader = 17;
            pointCloud->pointSize = 1.0f;
            pointCloud->brightness = 1.25f;
            pointCloud->color = point3d(1.0f, 0.95f, 0.85f);
            pointCloud->frustumRadius = 12.0f;
            pointCloud->compress = RenderCompress::x2;

            SkeletalAnimationComponent* animComp = e->AddComponent<SkeletalAnimationComponent>();
            animComp->skeleton = skeleton;
            animComp->animationClips = animations;
            animComp->rootMotionMode = RootMotionMode::Accumulate;
            animComp->CaptureBindPose();
            if (animations && !animations->empty())
            {
                animComp->SetAnimationByIndex(0, true);
                if (preferredClip && preferredClip[0] != '\0')
                {
                    animComp->SetAnimationByName(preferredClip, true);
                }
            }
            animComp->isPlaying = autoPlay && animComp->animationClip != nullptr;
            animComp->isLooping = true;
            animComp->currentTime = 0.0f;
            return e;
        };

    m_TestAnimEntity = nullptr;
    if (m_TestAnimEntity)
    {
        if (SkeletalAnimationComponent* animComp = m_TestAnimEntity->GetComponent<SkeletalAnimationComponent>())
        {
            animComp->ResetToBindPose();
            animComp->animationClip = nullptr;
            animComp->isPlaying = false;
            animComp->currentTime = 0.0f;
        }
    }

    m_IsActive = true;
}

void GameplayState::Exit()
{
    if (playerController)
    {
        playerController->Shutdown();
        delete playerController;
        playerController = nullptr;
    }

    if (m_World)
    {
        m_World->Shutdown();
        delete m_World;
        m_World = nullptr;
    }

    if (collisionManager)
    {
        collisionManager->Shutdown();
        delete collisionManager;
        collisionManager = nullptr;
    }

    if (questManager)
    {
        questManager->Shutdown();
        delete questManager;
        questManager = nullptr;
    }

    if (mouse)
    {
        delete mouse;
        mouse = nullptr;
    }

    if (window)
    {
        delete window;
        window = nullptr;
    }

    if (m_BoneBuffer)
    {
        m_BoneBuffer->Release();
        m_BoneBuffer = nullptr;
    }

    m_IsActive = false;
}

void GameplayState::LoadModels()
{
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\FistBasePos.obj");
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Arrow.obj");
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesBody.obj");
    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesArmor.obj");
    //Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\Fox.glb", m_FoxMesh, m_FoxSkeleton, m_FoxAnimations);
    //Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\CesiumMan.glb", m_CesiumMesh, m_CesiumSkeleton, m_CesiumAnimations);
    //Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\PunchComboNew.glb", m_PunchComboNewMesh, m_PunchComboNewSkeleton, m_PunchComboNewAnimations);

    //if (Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\TestAnims\\1.glb", m_TestAnimMesh, m_TestAnimSkeleton, m_TestAnimAnimations))
    //{
    //    Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\2.glb", m_TestAnimSkeleton, m_TestAnimAnimations, false);
    //    Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\3.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
    //    Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\4.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
    //}

    Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\SnakeModel.obj");
}

void GameplayState::InitSystems()
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

void GameplayState::Update()
{
    mouse->UpdateSystemCursorVisibility();
    if (!window->IsActive())
        return;

    mouse->Update();
    UpdateTestAnimationToggle();
    interp::UpdateTweens();

    playerController->ProcessInput();
    playerController->ProcessMouse();
    playerController->abilities->Update();
    playerController->ProccessUI();
    questManager->UpdateQuests();

    // Обработка QTE
    if (m_QTESystem.IsActive())
    {
        static bool wasRPressed = false;
        bool isRPressed = input::IsKeyPressed('R');

        if (isRPressed && !wasRPressed)
            m_QTESystem.OnKeyPressed('R');
        else if (!isRPressed && wasRPressed)
            m_QTESystem.OnKeyReleased('R');

        wasRPressed = isRPressed;
    }

    m_QTESystem.Update(1.0f / 60.0f);

    // Проверка смерти игрока
    Entity* player = m_World->entityStorage->GetEntityByName("Player");
    if (IsEntityValid(player) && !m_ShowVictoryMessage)
    {
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && playerHealth->hp <= 0 && !m_ShowGameOverMessage)
        {
            m_ShowGameOverMessage = true;
            m_MessageTimer = 3.0f;

            Entity* msgContainer = m_World->entityStorage->CreateEntity("GameOverMsg", nullptr);
            Transform2D* msgTransform = msgContainer->AddComponent<Transform2D>();
            msgTransform->anchorPoint = point3d(0, 0, 0);
            msgTransform->ratio = ScreenAspectRatio::XY;
            msgTransform->position = point3d(-0.35f, -0.1f, 0);

            TextLabel* text = msgContainer->AddComponent<TextLabel>();
            text->textW = L"ВЫ ПОГИБЛИ";
            text->fontFamilyW = L"Impact";
            text->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
            text->fontWeight = 900;
            text->fontSizePx = 70;
            text->fontScale = 1.2f;
            text->color = point3d(1.0f, 0.2f, 0.2f);

            DelayedDestroy* delayed = msgContainer->AddComponent<DelayedDestroy>();
            delayed->lifeTime = 3000;
        }
    }

    // Проверка босса и активация QTE
    if (m_CurrentBoss && m_CurrentBoss->IsActive() && !m_IsExecutionActive)
    {
        Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
        if (bossHealth)
        {
            float healthPercent = bossHealth->hp / bossHealth->maxHp;
            if (healthPercent <= 0.05f)
            {
                TriggerExecution();
            }
        }
    }

    // Логика во время QTE
    if (m_IsExecutionActive && m_CurrentBoss)
    {
        m_ExecutionTimer -= 1.0f / 60.0f;

        if (m_ExecutionUI)
        {
            UpdateExecutionTimerUI();
        }

        if (m_ExecutionTimer <= 0 && m_QTESystem.IsActive())
        {
            m_QTESystem.StopQTE();
            BossRecovery();
        }
    }

    // Тестовая кнопка урона боссу
    if (input::IsKeyDown('K'))
    {
        if (m_CurrentBoss && m_CurrentBoss->IsActive())
        {
            Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
            if (bossHealth)
            {
                bossHealth->hp -= 100.0f;
            }
        }
    }

    // Проверка победы
    static bool victoryShown = false;
    if (m_CurrentBoss && m_CurrentBoss->IsActive())
    {
        Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
        if (bossHealth && bossHealth->hp <= 0 && !victoryShown)
        {
            victoryShown = true;

            Entity* msg = m_World->entityStorage->CreateEntity("VictoryMsg", nullptr);
            Transform2D* t = msg->AddComponent<Transform2D>();
            t->anchorPoint = point3d(0, 0, 0);
            t->ratio = ScreenAspectRatio::XY;
            t->position = point3d(-0.25f, -0.1f, 0);

            TextLabel* text = msg->AddComponent<TextLabel>();
            text->textW = L"ПОБЕДА!";
            text->fontFamilyW = L"Impact";
            text->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
            text->fontWeight = 900;
            text->fontSizePx = 80;
            text->fontScale = 1.5f;
            text->color = point3d(0.2f, 0.8f, 0.2f);

            DelayedDestroy* d = msg->AddComponent<DelayedDestroy>();
            d->lifeTime = 3000;

            AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
            if (bossAI) bossAI->enabled = false;
        }
    }

    // Обновление таймера сообщений
    if (m_ShowGameOverMessage || m_ShowVictoryMessage)
    {
        m_MessageTimer -= 1.0f / 60.0f;
        if (m_MessageTimer <= 0)
        {
            m_ShowGameOverMessage = false;
            m_ShowVictoryMessage = false;
        }
    }

    // Обновление UI босса
    if (m_CurrentBoss && m_CurrentBoss->IsActive() && m_BossHealthFill) {
        Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
        if (bossHealth) {
            Entity* bossContainer = m_BossHealthFill->GetParent();
            if (bossContainer) bossContainer->SetActive(true);

            float healthPercent = bossHealth->hp / bossHealth->maxHp;
            Transform2D* barTransform = m_BossHealthFill->GetComponent<Transform2D>();
            if (barTransform) {
                barTransform->scale.x = 0.5f * healthPercent;
            }

            Rect* barRect = m_BossHealthFill->GetComponent<Rect>();
            if (barRect) {
                if (healthPercent > 0.6f) barRect->color = point3d(0.2f, 0.8f, 0.2f);
                else if (healthPercent > 0.3f) barRect->color = point3d(0.8f, 0.8f, 0.2f);
                else barRect->color = point3d(0.8f, 0.2f, 0.2f);
            }

            if (m_BossNumbersText) {
                wchar_t buffer[64];
                swprintf(buffer, 64, L"%.0f / %.0f", bossHealth->hp, bossHealth->maxHp);
                m_BossNumbersText->textW = buffer;
            }
        }
    }
    else {
        if (m_BossHealthFill) {
            Entity* bossContainer = m_BossHealthFill->GetParent();
            if (bossContainer) bossContainer->SetActive(false);
        }
    }

    // Границы арены
    if (m_CurrentBoss && m_CurrentBoss->IsActive())
    {
        Entity* player = m_World->entityStorage->GetEntityByName("Player");
        if (player)
        {
            Transform* playerTransform = player->GetComponent<Transform>();
            if (playerTransform)
            {
                float dx = playerTransform->position.x - m_BossArenaCenter.x;
                float dz = playerTransform->position.z - m_BossArenaCenter.z;
                float dist = sqrt(dx * dx + dz * dz);

                if (dist < m_BossArenaRadius && !m_IsInBossArena)
                {
                    m_IsInBossArena = true;
                    AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
                    if (bossAI) bossAI->enabled = true;
                    Entity* bossUIContainer = m_World->entityStorage->GetEntityByName("BossUIContainer");
                    if (bossUIContainer) bossUIContainer->SetActive(true);
                }

                if (m_IsInBossArena)
                {
                    if (playerTransform->position.x < -50.0f) playerTransform->position.x = -50.0f;
                    if (playerTransform->position.x > 50.0f) playerTransform->position.x = 50.0f;
                    if (playerTransform->position.z < 150.0f) playerTransform->position.z = 150.0f;
                    if (playerTransform->position.z > 250.0f) playerTransform->position.z = 250.0f;
                }
            }
        }
    }
    else
    {
        m_IsInBossArena = false;
    }

    ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

    m_World->UpdateCompute();
    m_World->UpdatePhysic();
    playerController->ProcessCamera();
}

void GameplayState::Render()
{
    ConstBuf::frame.aspect = XMFLOAT4{
        float(window->aspect), float(window->iaspect),
        float(window->width), float(window->height)
    };

    m_World->UpdateRender();
    mouse->RenderCursor();
}

void GameplayState::UpdateTestAnimationToggle()
{
    const bool isTogglePressed = input::IsKeyDown('Y');
    if (!isTogglePressed)
    {
        m_WasToggleAnimationPressed = false;
        return;
    }

    if (m_WasToggleAnimationPressed)
    {
        return;
    }

    m_WasToggleAnimationPressed = true;

    if (!m_TestAnimEntity)
    {
        return;
    }

    SkeletalAnimationComponent* animComp = m_TestAnimEntity->GetComponent<SkeletalAnimationComponent>();
    if (!animComp)
    {
        return;
    }

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

Entity* GameplayState::CreatePlayer(Entity* folder)
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

void GameplayState::CreateUI()
{
    Entity* entity;
    Transform2D* transform2D;
    Rect* rect;
    Button* button;
    TextLabel* textLabel;

    Entity* uiFolder = m_World->entityStorage->CreateEntity("UI");

    entity = m_World->entityStorage->CreateEntity("HealthHolder", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.9f, -0.6f, 0.0f);
    transform2D->scale = point3d(0.18f, 0.04f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.5f, 0.5f, 0.5f);
    rect->opacity = 0.5f;

    entity = m_World->entityStorage->CreateEntity("HealthBar", entity);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    rect = entity->AddComponent<Rect>();

    entity = m_World->entityStorage->CreateEntity("StaminaHolder", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.9f, -0.7f, 0.0f);
    transform2D->scale = point3d(0.18f, 0.02f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.5f, 0.5f, 0.5f);
    rect->opacity = 0.5f;

    entity = m_World->entityStorage->CreateEntity("StaminaBar", entity);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.8f, 0.8f, 1);

    entity = m_World->entityStorage->CreateEntity("HealthLabel", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.9f, -0.64f, 0.0f);
    textLabel = entity->AddComponent<TextLabel>();
    textLabel->textW = L"ЗДОРОВЬЕ";
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 44;
    textLabel->fontScale = 0.40f;
    textLabel->letterSpacingPx = 1.0f;

    entity = m_World->entityStorage->CreateEntity("StaminaLabel", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.9f, -0.74f, 0.0f);
    textLabel = entity->AddComponent<TextLabel>();
    textLabel->textW = L"ВЫНОСЛИВОСТЬ";
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 38;
    textLabel->fontScale = 0.34f;
    textLabel->letterSpacingPx = 1.0f;

    entity = m_World->entityStorage->CreateEntity("ElementLabel", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(0.8f, -0.64f, 0.0f);
    textLabel = entity->AddComponent<TextLabel>();
    textLabel->textW = L"НЕТ";
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 44;
    textLabel->fontScale = 0.40f;
    textLabel->letterSpacingPx = 1.0f;

    // UI Prototypes
    entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XX;
    transform2D->position = point3d(-0.9f, -0.85f, 0.0f);
    transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.75f, 0.0f, 0.0f);
    rect->cornerRadius = 1.f;

    entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XX;
    transform2D->position = point3d(-0.75f, -0.85f, 0.0f);
    transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.75f, 0.0f, 0.0f);
    rect->cornerRadius = 1.f;

    entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XX;
    transform2D->position = point3d(-0.9f, 0.85f, 0.0f);
    transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.75f, 0.0f, 0.0f);

    entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XX;
    transform2D->position = point3d(-0.75f, 0.85f, 0.0f);
    transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.75f, 0.0f, 0.0f);

    entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XX;
    transform2D->position = point3d(0, 0.85f, 0.0f);
    transform2D->scale = point3d(0.2f, 0.05f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.75f, 0.0f, 0.0f);

    entity = m_World->entityStorage->CreateEntity("BossHealth", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(0, 0.7f, 0.0f);
    transform2D->scale = point3d(0.5f, 0.025f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.75f, 0.0f, 0.0f);

    // Boss UI Container
    Entity* bossUIContainer = m_World->entityStorage->CreateEntity("BossUIContainer", uiFolder);

    entity = m_World->entityStorage->CreateEntity("BossHealthBg", bossUIContainer);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(0, 0.7f, 0.0f);
    transform2D->scale = point3d(0.5f, 0.035f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.2f, 0.2f, 0.2f);
    rect->opacity = 0.7f;

    entity = m_World->entityStorage->CreateEntity("BossHealthBar", bossUIContainer);
    m_BossHealthFill = entity;
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.5f, 0.7f, 0.0f);
    transform2D->scale = point3d(0.5f, 0.035f, 0.0f);
    rect = entity->AddComponent<Rect>();
    rect->color = point3d(0.8f, 0.2f, 0.2f);

    entity = m_World->entityStorage->CreateEntity("BossName", bossUIContainer);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.16f, 0.91f, 0.0f);
    textLabel = entity->AddComponent<TextLabel>();
    textLabel->textW = L"ГИПЕРЗВЕЗДА";
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 52;
    textLabel->fontScale = 1.0f;
    textLabel->color = point3d(1.0f, 0.85f, 0.2f);
    textLabel->opacity = 1.0f;

    entity = m_World->entityStorage->CreateEntity("BossHealthNumbers", bossUIContainer);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 0, 0);
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(-0.1f, 0.74f, 0.0f);
    m_BossNumbersText = entity->AddComponent<TextLabel>();
    m_BossNumbersText->textW = L"2000 / 2000";
    m_BossNumbersText->fontFamilyW = L"Impact";
    m_BossNumbersText->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    m_BossNumbersText->fontWeight = 700;
    m_BossNumbersText->fontSizePx = 36;
    m_BossNumbersText->fontScale = 1.0f;
    m_BossNumbersText->color = point3d(1.0f, 1.0f, 1.0f);
    m_BossNumbersText->opacity = 0.9f;

    bossUIContainer->SetActive(false);

    entity = m_World->entityStorage->CreateEntity("ExecutionLabel", uiFolder);
    transform2D = entity->AddComponent<Transform2D>();
    transform2D->ratio = ScreenAspectRatio::XY;
    transform2D->position = point3d(0.9f, -0.74f, 0.0f);
    textLabel = entity->AddComponent<TextLabel>();
    textLabel->textW = L"НИЧЕГО";
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 38;
    textLabel->fontScale = 0.34f;
    textLabel->letterSpacingPx = 1.0f;
}

void GameplayState::CreateSpaceBackground(Entity* folder, int quality)
{
    int outerSpace_cnt = 6853 / quality;

    Entity* entity = m_World->entityStorage->CreateEntity("OuterSpace", folder);

    Nebula* nebula = entity->AddComponent<Nebula>();
    nebula->vShader = 25;
    nebula->count = outerSpace_cnt;
    nebula->mode = pMode::point;
    nebula->isOnBackground = true;
}

void GameplayState::CreateAries(Entity* folder)
{
    Entity* aries = m_World->entityStorage->CreateEntity("Aries", folder);
    Transform* transform = aries->AddComponent<Transform>();
    transform->scale = point3d(4, 4, 4);
    transform->position = point3d(0.0f, 20.0f, 50.0f);
}

void GameplayState::CreateZenithLocation(Entity* folder, int quality)
{
    Entity* entity;
    Nebula* nebula;
    Transform* transform;
    SphereCollider* sphereCollider;

    int pillars_cnt = 3725470 / 2 / quality;
    int galaxy_cnt = 182361 / quality;

    Entity* location = m_World->entityStorage->CreateEntity("Zenith location", folder);

    transform = location->AddComponent<Transform>();
    transform->position = point3d(0, 0, 200);

    // Pillars hand | point
    entity = m_World->entityStorage->CreateEntity("PHP", folder);
    transform = entity->AddComponent<Transform>();

    nebula = location->AddComponent<Nebula>();
    nebula->vShader = 24;
    nebula->count = pillars_cnt;
    nebula->mode = pMode::point;
    nebula->scale = 5;
    nebula->frustumRadius = 40;

    // Inside nebula | point
    entity = m_World->entityStorage->CreateEntity("INP", location);
    transform = entity->AddComponent<Transform>();
    transform->position = point3d(150, 0, 0);

    nebula = entity->AddComponent<Nebula>();
    nebula->vShader = 23;
    nebula->count = pillars_cnt;
    nebula->mode = pMode::point;
    nebula->color = point3d(0.3, 0.5, 1.0);
    nebula->scale = 10;
    nebula->frustumRadius = 40;

    // Pillars hand | glow
    entity = m_World->entityStorage->CreateEntity("PHG", location);
    transform = entity->AddComponent<Transform>();

    nebula = entity->AddComponent<Nebula>();
    nebula->vShader = 24;
    nebula->count = pillars_cnt;
    nebula->skipper = 1394 / 2;
    nebula->mode = pMode::glow;
    nebula->scale = 5;
    nebula->frustumRadius = 40;

    // Inside nebula | glow
    entity = m_World->entityStorage->CreateEntity("ING", location);
    transform = entity->AddComponent<Transform>();
    transform->position = point3d(150, 0, 0);

    nebula = entity->AddComponent<Nebula>();
    nebula->vShader = 23;
    nebula->count = pillars_cnt;
    nebula->skipper = 1394;
    nebula->mode = pMode::glow;
    nebula->color = point3d(0.3, 0.5, 1.0);
    nebula->scale = 10;
    nebula->frustumRadius = 40;

    // 2 Nebula
    entity = m_World->entityStorage->CreateEntity("INP1", location);
    transform = entity->AddComponent<Transform>();
    transform->position = point3d(0, 0, 0);

    nebula = entity->AddComponent<Nebula>();
    nebula->vShader = 23;
    nebula->count = pillars_cnt;
    nebula->mode = pMode::point;
    nebula->color = point3d(0.8, 0.4, 0.2);
    nebula->scale = 10;
    nebula->frustumRadius = 40;

    entity = m_World->entityStorage->CreateEntity("ING1", location);
    transform = entity->AddComponent<Transform>();
    transform->position = point3d(0, 0, 0);

    nebula = entity->AddComponent<Nebula>();
    nebula->vShader = 23;
    nebula->count = pillars_cnt;
    nebula->skipper = 1394;
    nebula->mode = pMode::glow;
    nebula->color = point3d(0.8, 0.4, 0.2);
    nebula->scale = 10;
    nebula->frustumRadius = 40;

    CreateArenaBarrier(location, point3d(0, 0, 0), 52.0f, 200);
    m_BossArenaCenter = point3d(0, 0, 200);
    m_BossArenaRadius = 60.0f;

    // Создание босса
    Entity* BossEntity = m_World->entityStorage->CreateEntity("BossEnemy", location);
    m_CurrentBoss = BossEntity;

    Transform* bossTransform = BossEntity->AddComponent<Transform>();
    bossTransform->position = point3d(0.0f, 0.0f, 0.0f);

    sphereCollider = BossEntity->AddComponent<SphereCollider>();
    sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
    sphereCollider->radius = 3.5f;
    BossEntity->AddComponent<CameraTarget>();

    Health* health = BossEntity->AddComponent<Health>();
    health->fraction = Fraction::Enemy;
    health->maxHp = 2000.0f;
    health->hp = 2000.0f;

    PhysicBody* testPhysic = BossEntity->AddComponent<PhysicBody>();
    testPhysic->airFriction = 0.1f;
    testPhysic->mass = 100.0f;
    testPhysic->velocity = point3d(0.0f, 0.0f, 0.0f);

    Star* testStar = BossEntity->AddComponent<Star>();
    testStar->radius = 3.0f;
    testStar->crownRadius = 3.5f;
    testStar->color1 = point3d(0.8f, 0.2f, 0.8f);
    testStar->color2 = point3d(0.5f, 0.1f, 0.5f);
    testStar->crownColor = point3d(0.3f, 0.6f, 0.8f);

    AIComponent* ai = BossEntity->AddComponent<AIComponent>();
    ai->enabled = true;
    ai->behaviorType = AIBehaviorType::BOSS_PHASE_1;
    ai->detectionRange = 50.0f;
    ai->chaseRange = 80.0f;
    ai->attackRange = 4.0f;
    ai->movementSpeed = 8.0f;
    ai->arrivalDistance = 2.0f;
    ai->accelerationStrength = 5.0f;
    ai->maxAcceleration = 20.0f;
    ai->attackCooldown = 1.2f;
    ai->attackDamage = 20.0f;
    ai->stateTimer = 0.0f;
    ai->searchDuration = 5.0f;
    ai->searchPatrolRadius = 15.0f;
    ai->targetId = -1;

    ai->patrolPoints = {
        point3d(-5.0f, 0.0f, -5.0f),
        point3d(5.0f, 0.0f, -5.0f),
        point3d(5.0f, 0.0f, 5.0f),
        point3d(-5.0f, 0.0f, 5.0f)
    };
    ai->currentPatrolIndex = 0;

    ai->visual.originalRadius = testStar->radius;
    ai->visual.originalColor = testStar->color1;
    ai->visual.attackScale = 1.5f;
    ai->visual.attackDuration = 0.3f;
    ai->visual.aoePulseSpeed = 5.0f;

    BossComponent* boss = BossEntity->AddComponent<BossComponent>();
    boss->currentPhase = 1;
    boss->phaseHealthThresholds[0] = 0.7f;
    boss->phaseHealthThresholds[1] = 0.3f;
    boss->arenaMinX = -50.0f;
    boss->arenaMaxX = 50.0f;
    boss->arenaMinY = -50.0f;
    boss->arenaMaxY = 50.0f;
    boss->arenaMinZ = -50.0f;
    boss->arenaMaxZ = 50.0f;
    boss->dashCooldown = 3.0f;
    boss->dashSpeed = 100.0f;
    boss->dashDamage = 5.0f;
    boss->starShotCooldown = 5.5f;
    boss->starShotCount = 20.f;
    boss->starShotSpeed = 15.0f;
    boss->starShotDamage = 0.5f;
    boss->sideDashCooldown = 4.5f;
    boss->sideDashSpeed = 10.0f;
    boss->aoeAttackRange = 12.0f;
    boss->aoeDamage = 15.0f;
    boss->specialAttackCooldown = 10.0f;
    boss->rageSpeedMultiplier = 1.0f;
    boss->lastSpecialAttackTime = 0.0f;
    boss->lastDashTime = 0.0f;
    boss->lastSideDashTime = 0.0f;
    boss->lastStarShotTime = 0.0f;
}

void GameplayState::CreateArenaBarrier(Entity* parent, const point3d& center, float radius, int starCount)
{
    Entity* barrier = m_World->entityStorage->CreateEntity("ArenaBarrier", parent);
    barrier->AddComponent<Transform>();

    float heightMin = -40.0f;
    float heightMax = 40.0f;
    int starsPerCircle = starCount / 4;

    for (int i = 0; i < starsPerCircle; i++)
    {
        float angle = (i * 2.0f * PI / starsPerCircle);
        float x = center.x + cos(angle) * radius;
        float z = center.z + sin(angle) * radius;

        Entity* starEntity = m_World->entityStorage->CreateEntity("BarrierStar", barrier);
        Transform* starTransform = starEntity->AddComponent<Transform>();
        starTransform->position = point3d(x, center.y + heightMin, z);

        Star* star = starEntity->AddComponent<Star>();
        star->radius = 0.6f;
        star->crownRadius = 0.9f;
        star->color1 = point3d(0.3f, 0.6f, 1.0f);
        star->color2 = point3d(0.1f, 0.3f, 0.8f);
        star->crownColor = point3d(0.5f, 0.8f, 1.0f);

        ParticleEmitter* particles = starEntity->AddComponent<ParticleEmitter>();
        particles->rate = 30;
        particles->lifetime = 500;
        particles->color = point3d(0.3f, 0.6f, 1.0f);
        particles->size = { 0.1f, 0.3f };
        particles->opacity = { 0.8f, 0.0f };
        particles->emitDirection = EmitDirection::Up;
        particles->spread = { 1.57f, 1.57f };
        particles->speed = { 2.0f, 1.0f };
    }

    for (int i = 0; i < starsPerCircle; i++)
    {
        float angle = (i * 2.0f * PI / starsPerCircle);
        float x = center.x + cos(angle) * radius;
        float z = center.z + sin(angle) * radius;

        Entity* starEntity = m_World->entityStorage->CreateEntity("BarrierStar", barrier);
        Transform* starTransform = starEntity->AddComponent<Transform>();
        starTransform->position = point3d(x, center.y + heightMax, z);

        Star* star = starEntity->AddComponent<Star>();
        star->radius = 0.6f;
        star->crownRadius = 0.9f;
        star->color1 = point3d(1.0f, 0.4f, 0.8f);
        star->color2 = point3d(0.8f, 0.2f, 0.5f);
        star->crownColor = point3d(1.0f, 0.6f, 1.0f);

        ParticleEmitter* particles = starEntity->AddComponent<ParticleEmitter>();
        particles->rate = 30;
        particles->lifetime = 500;
        particles->color = point3d(1.0f, 0.4f, 0.8f);
        particles->size = { 0.1f, 0.3f };
        particles->opacity = { 0.8f, 0.0f };
        particles->emitDirection = EmitDirection::Up;
        particles->spread = { 1.57f, 1.57f };
        particles->speed = { 2.0f, 1.0f };
    }

    for (int i = 0; i < starsPerCircle; i++)
    {
        float angle1 = (i * 2.0f * PI / starsPerCircle);
        float angle2 = ((i + 1) * 2.0f * PI / starsPerCircle);

        float x1 = center.x + cos(angle1) * radius;
        float z1 = center.z + sin(angle1) * radius;
        float x2 = center.x + cos(angle2) * radius;
        float z2 = center.z + sin(angle2) * radius;

        Entity* beamEntity = m_World->entityStorage->CreateEntity("BarrierBeam", barrier);
        beamEntity->AddComponent<Transform>();

        Beam* beam = beamEntity->AddComponent<Beam>();
        beam->point1 = point3d(x1, center.y + heightMin, z1);
        beam->point2 = point3d(x2, center.y + heightMin, z2);
        beam->size1 = 0.1f;
        beam->size2 = 0.1f;
        beam->color1 = point3d(0.3f, 0.5f, 0.9f);
        beam->color2 = point3d(0.3f, 0.5f, 0.9f);
        beam->opacity1 = 0.5f;
        beam->opacity2 = 0.5f;
    }
}

Entity* GameplayState::CreateMinion(point3d position, float startDelay)
{
    Entity* minion = m_World->entityStorage->CreateEntity("MinionEnemy", m_WorldFolder);

    Transform* minionTransform = minion->AddComponent<Transform>();
    minionTransform->position = position;

    SphereCollider* minionCollider = minion->AddComponent<SphereCollider>();
    minionCollider->collisionGroup = CollisionFilter::Group::Enemy;
    minionCollider->radius = 1.0f;

    PhysicBody* minionPhysic = minion->AddComponent<PhysicBody>();
    minionPhysic->airFriction = 0.1f;

    Health* minionHealth = minion->AddComponent<Health>();
    minionHealth->fraction = Fraction::Enemy;
    minionHealth->maxHp = 50.0f;
    minionHealth->hp = 50.0f;

    Star* minionStar = minion->AddComponent<Star>();
    minionStar->radius = 1.0f;
    minionStar->crownRadius = 1.2f;
    minionStar->color1 = point3d(0.5f, 0.5f, 0.5f);

    AIComponent* minionAI = minion->AddComponent<AIComponent>();
    minionAI->enabled = true;
    minionAI->isMinion = true;
    minionAI->minionStartDelay = startDelay;
    minionAI->behaviorType = AIBehaviorType::PATROL;

    return minion;
}

void GameplayState::CreateNebula(Entity* folder, int quality)
{
    // Ваш существующий код CreateNebula
}

void GameplayState::CreateStarQuestLoc(Entity* folder, int quality)
{
    // Ваш существующий код CreateStarQuestLoc
}

void GameplayState::ShowGameOverMessage(const wchar_t* message, const point3d& color)
{
    Entity* gameOverContainer = m_World->entityStorage->CreateEntity("GameOverContainer", nullptr);
    Transform2D* containerTransform = gameOverContainer->AddComponent<Transform2D>();
    containerTransform->anchorPoint = point3d(0, 0, 0);
    containerTransform->ratio = ScreenAspectRatio::XY;
    containerTransform->position = point3d(0, 0, 0);
    containerTransform->scale = point3d(1, 1, 0);

    Entity* bg = m_World->entityStorage->CreateEntity("GameOverBG", gameOverContainer);
    Transform2D* bgTransform = bg->AddComponent<Transform2D>();
    bgTransform->anchorPoint = point3d(0, 0, 0);
    bgTransform->ratio = ScreenAspectRatio::XY;
    bgTransform->position = point3d(-0.5f, -0.2f, 0);
    bgTransform->scale = point3d(1, 0.4f, 0);
    Rect* bgRect = bg->AddComponent<Rect>();
    bgRect->color = point3d(0, 0, 0);
    bgRect->opacity = 0.8f;

    Entity* text = m_World->entityStorage->CreateEntity("GameOverText", gameOverContainer);
    Transform2D* textTransform = text->AddComponent<Transform2D>();
    textTransform->anchorPoint = point3d(0, 0, 0);
    textTransform->ratio = ScreenAspectRatio::XY;
    textTransform->position = point3d(-0.25f, -0.1f, 0);
    TextLabel* textLabel = text->AddComponent<TextLabel>();
    textLabel->textW = message;
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 900;
    textLabel->fontSizePx = 80;
    textLabel->fontScale = 1.5f;
    textLabel->color = color;
    textLabel->opacity = 1.0f;

    DelayedDestroy* delayed = gameOverContainer->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 3000;
}

void GameplayState::ShowExecutionUI()
{
    if (m_ExecutionUI) return;

    m_ExecutionUI = m_World->entityStorage->CreateEntity("ExecutionUI", nullptr);

    Entity* bg = m_World->entityStorage->CreateEntity("ExecutionBG", m_ExecutionUI);
    Transform2D* bgTransform = bg->AddComponent<Transform2D>();
    bgTransform->anchorPoint = point3d(0, 0, 0);
    bgTransform->ratio = ScreenAspectRatio::XY;
    bgTransform->position = point3d(-0.4f, -0.2f, 0);
    bgTransform->scale = point3d(0.8f, 0.4f, 0);
    Rect* bgRect = bg->AddComponent<Rect>();
    bgRect->color = point3d(0, 0, 0);
    bgRect->opacity = 0.85f;
    bgRect->cornerRadius = 0.05f;

    Entity* text1 = m_World->entityStorage->CreateEntity("ExecutionText1", m_ExecutionUI);
    Transform2D* t1 = text1->AddComponent<Transform2D>();
    t1->anchorPoint = point3d(0, 0, 0);
    t1->ratio = ScreenAspectRatio::XY;
    t1->position = point3d(-0.3f, -0.05f, 0);
    TextLabel* label1 = text1->AddComponent<TextLabel>();
    label1->textW = L"НАЖМИ";
    label1->fontFamilyW = L"Impact";
    label1->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    label1->fontWeight = 700;
    label1->fontSizePx = 50;
    label1->fontScale = 1.0f;
    label1->color = point3d(1.0f, 1.0f, 1.0f);

    Entity* keyEntity = m_World->entityStorage->CreateEntity("ExecutionKey", m_ExecutionUI);
    Transform2D* keyTransform = keyEntity->AddComponent<Transform2D>();
    keyTransform->anchorPoint = point3d(0, 0, 0);
    keyTransform->ratio = ScreenAspectRatio::XY;
    keyTransform->position = point3d(-0.05f, -0.05f, 0);
    TextLabel* keyLabel = keyEntity->AddComponent<TextLabel>();
    keyLabel->textW = L"R";
    keyLabel->fontFamilyW = L"Impact";
    keyLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    keyLabel->fontWeight = 900;
    keyLabel->fontSizePx = 70;
    keyLabel->fontScale = 1.5f;
    keyLabel->color = point3d(1.0f, 0.8f, 0.2f);

    Entity* text2 = m_World->entityStorage->CreateEntity("ExecutionText2", m_ExecutionUI);
    Transform2D* t2 = text2->AddComponent<Transform2D>();
    t2->anchorPoint = point3d(0, 0, 0);
    t2->ratio = ScreenAspectRatio::XY;
    t2->position = point3d(0.08f, -0.05f, 0);
    TextLabel* label2 = text2->AddComponent<TextLabel>();
    label2->textW = L"ДЛЯ КАЗНИ";
    label2->fontFamilyW = L"Impact";
    label2->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    label2->fontWeight = 700;
    label2->fontSizePx = 50;
    label2->fontScale = 1.0f;
    label2->color = point3d(1.0f, 1.0f, 1.0f);

    Entity* timerText = m_World->entityStorage->CreateEntity("ExecutionTimer", m_ExecutionUI);
    Transform2D* timerTransform = timerText->AddComponent<Transform2D>();
    timerTransform->anchorPoint = point3d(0, 0, 0);
    timerTransform->ratio = ScreenAspectRatio::XY;
    timerTransform->position = point3d(-0.05f, -0.25f, 0);
    TextLabel* timerLabel = timerText->AddComponent<TextLabel>();
    timerLabel->textW = L"5.0";
    timerLabel->fontFamilyW = L"Impact";
    timerLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    timerLabel->fontWeight = 700;
    timerLabel->fontSizePx = 40;
    timerLabel->fontScale = 1.0f;
    timerLabel->color = point3d(1.0f, 1.0f, 1.0f);
}

void GameplayState::HideExecutionUI()
{
    if (m_ExecutionUI)
    {
        m_ExecutionUI->SetActive(false);
        m_ExecutionUI = nullptr;
    }
}

void GameplayState::ExecuteBoss()
{
    if (!m_CurrentBoss) return;

    Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
    if (bossHealth)
    {
        bossHealth->hp = 0;
    }

    HideExecutionUI();
    m_IsExecutionActive = false;

    Entity* msg = m_World->entityStorage->CreateEntity("VictoryMsg", nullptr);
    Transform2D* t = msg->AddComponent<Transform2D>();
    t->anchorPoint = point3d(0, 0, 0);
    t->ratio = ScreenAspectRatio::XY;
    t->position = point3d(-0.25f, -0.1f, 0);

    TextLabel* text = msg->AddComponent<TextLabel>();
    text->textW = L"ПОБЕДА!";
    text->fontFamilyW = L"Impact";
    text->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    text->fontWeight = 900;
    text->fontSizePx = 80;
    text->fontScale = 1.5f;
    text->color = point3d(0.2f, 0.8f, 0.2f);

    DelayedDestroy* d = msg->AddComponent<DelayedDestroy>();
    d->lifeTime = 3000;

    AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
    if (bossAI) bossAI->enabled = false;
}

void GameplayState::BossRecovery()
{
    if (!m_CurrentBoss) return;

    m_IsExecutionActive = false;
    HideExecutionUI();

    Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
    if (bossHealth)
    {
        bossHealth->hp = bossHealth->maxHp * 0.2f;
    }

    AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
    if (bossAI) bossAI->enabled = true;

    Star* bossStar = m_CurrentBoss->GetComponent<Star>();
    if (bossStar)
    {
        bossStar->color1 = point3d(1.0f, 0.2f, 0.2f);
        bossStar->crownColor = point3d(0.8f, 0.3f, 0.3f);
        bossStar->radius = 5.0f;
    }
}

void GameplayState::UpdateExecutionTimerUI()
{
    if (!m_ExecutionUI) return;

    static float pulseTime = 0.0f;
    pulseTime += 1.0f / 60.0f;

    const std::vector<Entity*>& children = m_ExecutionUI->GetChildren();
    for (Entity* child : children)
    {
        if (child->name == "ExecutionKey")
        {
            Transform2D* transform = child->GetComponent<Transform2D>();
            TextLabel* label = child->GetComponent<TextLabel>();

            if (transform && label && m_QTESystem.IsActive())
            {
                float scale = 1.5f + 0.2f * sin(pulseTime * 10.0f);
                float progress = (float)m_CurrentQTE.currentPresses / m_CurrentQTE.requiredPresses;
                if (progress > 0.7f)
                {
                    scale = 1.5f + 0.35f * sin(pulseTime * 18.0f);
                }
                transform->scale = point3d(scale, scale, 1.0f);

                if (progress > 0.75f)
                    label->color = point3d(0.2f, 1.0f, 0.2f);
                else if (progress > 0.5f)
                    label->color = point3d(1.0f, 1.0f, 0.2f);
                else
                    label->color = point3d(1.0f, 0.8f, 0.2f);
            }
        }

        if (child->name == "ExecutionTimer")
        {
            TextLabel* timerLabel = child->GetComponent<TextLabel>();
            if (timerLabel)
            {
                wchar_t buffer[16];
                swprintf(buffer, 16, L"%.1f", m_ExecutionTimer);
                timerLabel->textW = buffer;

                if (m_ExecutionTimer < 2.0f)
                {
                    timerLabel->color = point3d(1.0f, 0.0f, 0.0f);
                    Transform2D* timerTransform = child->GetComponent<Transform2D>();
                    if (timerTransform)
                    {
                        float pulse = 1.0f + 0.1f * sin(pulseTime * 15.0f);
                        timerTransform->scale = point3d(pulse, pulse, 1.0f);
                    }
                }
                else if (m_ExecutionTimer < 4.0f)
                {
                    timerLabel->color = point3d(1.0f, 1.0f, 0.0f);
                }
                else
                {
                    timerLabel->color = point3d(1.0f, 1.0f, 1.0f);
                }
            }
        }
    }
}

void GameplayState::TriggerExecution()
{
    if (m_IsExecutionActive) return;

    Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
    if (!bossHealth || bossHealth->hp <= 0) return;

    m_IsExecutionActive = true;
    m_ExecutionTimer = 5.0f;

    AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
    if (bossAI) bossAI->enabled = false;

    Star* bossStar = m_CurrentBoss->GetComponent<Star>();
    if (bossStar)
    {
        bossStar->color1 = point3d(0.5f, 0.5f, 0.5f);
        bossStar->crownColor = point3d(0.3f, 0.3f, 0.3f);
    }

    ShowExecutionUI();

    m_CurrentQTE.type = QTEType::BUTTON_MASH;
    m_CurrentQTE.requiredKey = 'R';
    m_CurrentQTE.requiredPresses = 8;
    m_CurrentQTE.duration = 5.0f;

    m_CurrentQTE.onComplete = [this](bool success, bool isPerfect) {
        if (success)
        {
            ExecuteBoss();
        }
        else
        {
            BossRecovery();
        }
        };

    m_QTESystem.StartQTE(m_CurrentQTE);
}