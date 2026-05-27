//// GameStates/GameOverState.cpp
//#include "GameOverState.h"
//#include "..\\LevelManagerClass.h"
//
//GameOverState::GameOverState(LevelManagerClass* manager)
//    : m_Manager(manager)
//    , m_World(nullptr)
//    , m_GameOverUI(nullptr)
//    , m_IsActive(false)
//    , m_Timer(0.0f)
//{
//}
//
//GameOverState::~GameOverState()
//{
//    Exit();
//}
//
//void GameOverState::Enter()
//{
//    if (m_IsActive) return;
//
//    // Создаём корневой контейнер для UI
//    m_GameOverUI = m_World->entityStorage->CreateEntity("GameOverUI");
//
//    // Затемнение фона (красноватый оттенок)
//    Entity* bg = m_World->entityStorage->CreateEntity("GameOverBG", m_GameOverUI);
//    Transform2D* bgTr = bg->AddComponent<Transform2D>();
//    bgTr->anchorPoint = point3d(0, 0, 0);
//    bgTr->ratio = ScreenAspectRatio::XY;
//    bgTr->position = point3d(-0.5f, -0.5f, -0.1f);
//    bgTr->scale = point3d(1.0f, 1.0f, 0);
//
//    Rect* bgRect = bg->AddComponent<Rect>();
//    bgRect->color = point3d(0.15f, 0.02f, 0.02f);  // Тёмно-красный
//    bgRect->opacity = 0.85f;
//
//    // Декоративная рамка
//    Entity* border = m_World->entityStorage->CreateEntity("GameOverBorder", m_GameOverUI);
//    Transform2D* borderTr = border->AddComponent<Transform2D>();
//    borderTr->anchorPoint = point3d(0, 0, 0);
//    borderTr->ratio = ScreenAspectRatio::XY;
//    borderTr->position = point3d(-0.45f, -0.35f, -0.05f);
//    borderTr->scale = point3d(0.9f, 0.7f, 0);
//
//    Rect* borderRect = border->AddComponent<Rect>();
//    borderRect->color = point3d(0.8f, 0.1f, 0.1f);
//    borderRect->opacity = 0.3f;
//    borderRect->cornerRadius = 0.03f;
//    //borderRect->isFilled = false;  // Только рамка
//    //borderRect->borderWidth = 0.005f;
//
//    // Иконка черепа (используем текст как иконку)
//    Entity* skullIcon = m_World->entityStorage->CreateEntity("SkullIcon", m_GameOverUI);
//    Transform2D* skullTr = skullIcon->AddComponent<Transform2D>();
//    skullTr->anchorPoint = point3d(0, 0, 0);
//    skullTr->ratio = ScreenAspectRatio::XY;
//    skullTr->position = point3d(-0.08f, 0.15f, 0);
//
//    TextLabel* skullLabel = skullIcon->AddComponent<TextLabel>();
//    skullLabel->textW = L"💀";  // Череп (может не отобразиться, тогда замените на текст)
//    skullLabel->fontFamilyW = L"Impact";
//    skullLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
//    skullLabel->fontWeight = 900;
//    skullLabel->fontSizePx = 100;
//    skullLabel->fontScale = 1.5f;
//    skullLabel->color = point3d(1.0f, 0.1f, 0.1f);
//    skullLabel->opacity = 0.9f;
//
//    // Основной текст "ВЫ ПОГИБЛИ"
//    Entity* mainText = m_World->entityStorage->CreateEntity("GameOverMainText", m_GameOverUI);
//    Transform2D* mainTextTr = mainText->AddComponent<Transform2D>();
//    mainTextTr->anchorPoint = point3d(0, 0, 0);
//    mainTextTr->ratio = ScreenAspectRatio::XY;
//    mainTextTr->position = point3d(-0.25f, -0.05f, 0);
//
//    TextLabel* mainLabel = mainText->AddComponent<TextLabel>();
//    mainLabel->textW = L"ВЫ ПОГИБЛИ";
//    mainLabel->fontFamilyW = L"Impact";
//    mainLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
//    mainLabel->fontWeight = 900;
//    mainLabel->fontSizePx = 90;
//    mainLabel->fontScale = 1.2f;
//    mainLabel->color = point3d(1.0f, 0.15f, 0.15f);
//    mainLabel->letterSpacingPx = 3.0f;
//
//    // Эффект тени для основного текста
//    Entity* shadowText = m_World->entityStorage->CreateEntity("GameOverShadowText", m_GameOverUI);
//    Transform2D* shadowTr = shadowText->AddComponent<Transform2D>();
//    shadowTr->anchorPoint = point3d(0, 0, 0);
//    shadowTr->ratio = ScreenAspectRatio::XY;
//    shadowTr->position = point3d(-0.248f, -0.052f, -0.01f);
//
//    TextLabel* shadowLabel = shadowText->AddComponent<TextLabel>();
//    shadowLabel->textW = L"ВЫ ПОГИБЛИ";
//    shadowLabel->fontFamilyW = L"Impact";
//    shadowLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
//    shadowLabel->fontWeight = 900;
//    shadowLabel->fontSizePx = 90;
//    shadowLabel->fontScale = 1.2f;
//    shadowLabel->color = point3d(0, 0, 0);
//    shadowLabel->opacity = 0.5f;
//    shadowLabel->letterSpacingPx = 3.0f;
//
//    // Статистика (можно добавить позже)
//    Entity* statsText = m_World->entityStorage->CreateEntity("GameOverStats", m_GameOverUI);
//    Transform2D* statsTr = statsText->AddComponent<Transform2D>();
//    statsTr->anchorPoint = point3d(0, 0, 0);
//    statsTr->ratio = ScreenAspectRatio::XY;
//    statsTr->position = point3d(-0.2f, -0.2f, 0);
//
//    TextLabel* statsLabel = statsText->AddComponent<TextLabel>();
//    statsLabel->textW = L"Время в игре: --:--\nВрагов побеждено: 0\nУрона нанесено: 0";
//    statsLabel->fontFamilyW = L"Impact";
//    statsLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
//    statsLabel->fontWeight = 500;
//    statsLabel->fontSizePx = 24;
//    statsLabel->fontScale = 1.0f;
//    statsLabel->color = point3d(0.7f, 0.7f, 0.7f);
//    statsLabel->opacity = 0.7f;
//    statsLabel->lineSpacing = 1.5f;
//
//    // Кнопка "ПЕРЕЗАПУСК"
//    Entity* restartBtn = m_World->entityStorage->CreateEntity("RestartButton", m_GameOverUI);
//    Transform2D* restartBtnTr = restartBtn->AddComponent<Transform2D>();
//    restartBtnTr->anchorPoint = point3d(0, 0, 0);
//    restartBtnTr->ratio = ScreenAspectRatio::XY;
//    restartBtnTr->position = point3d(-0.2f, -0.45f, 0);
//    restartBtnTr->scale = point3d(0.4f, 0.06f, 0);
//
//    Rect* restartRect = restartBtn->AddComponent<Rect>();
//    restartRect->color = point3d(0.6f, 0.1f, 0.1f);
//    restartRect->cornerRadius = 0.03f;
//    restartRect->opacity = 0.8f;
//
//    /*Button* restartButton = restartBtn->AddComponent<Button>();
//    restartButton->onClick = [this]() {
//        m_Manager->RestartGame();
//        };
//
//    restartButton->onHoverEnter = [restartRect]() {
//        restartRect->color = point3d(0.9f, 0.2f, 0.2f);
//        restartRect->opacity = 1.0f;
//        };
//
//    restartButton->onHoverExit = [restartRect]() {
//        restartRect->color = point3d(0.6f, 0.1f, 0.1f);
//        restartRect->opacity = 0.8f;
//        };*/
//
//    // Текст на кнопке
//    Entity* restartText = m_World->entityStorage->CreateEntity("RestartBtnText", restartBtn);
//    Transform2D* restartTextTr = restartText->AddComponent<Transform2D>();
//    restartTextTr->anchorPoint = point3d(0, 0, 0);
//    restartTextTr->ratio = ScreenAspectRatio::XY;
//    restartTextTr->position = point3d(-0.1f, -0.44f, 0.01f);
//
//    TextLabel* restartLabel = restartText->AddComponent<TextLabel>();
//    restartLabel->textW = L"ПЕРЕЗАПУСК [ENTER]";
//    restartLabel->fontFamilyW = L"Impact";
//    restartLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
//    restartLabel->fontWeight = 700;
//    restartLabel->fontSizePx = 28;
//    restartLabel->fontScale = 1.0f;
//    restartLabel->color = point3d(1, 1, 1);
//
//    // Кнопка "В ГЛАВНОЕ МЕНЮ"
//    Entity* menuBtn = m_World->entityStorage->CreateEntity("MenuButton", m_GameOverUI);
//    Transform2D* menuBtnTr = menuBtn->AddComponent<Transform2D>();
//    menuBtnTr->anchorPoint = point3d(0, 0, 0);
//    menuBtnTr->ratio = ScreenAspectRatio::XY;
//    menuBtnTr->position = point3d(-0.15f, -0.55f, 0);
//    menuBtnTr->scale = point3d(0.3f, 0.05f, 0);
//
//    Rect* menuRect = menuBtn->AddComponent<Rect>();
//    menuRect->color = point3d(0.3f, 0.3f, 0.3f);
//    menuRect->cornerRadius = 0.03f;
//    menuRect->opacity = 0.7f;
//
//   /* Button* menuButton = menuBtn->AddComponent<Button>();
//    menuButton->onClick = [this]() {
//        m_Manager->SwitchToMainMenu();
//        };
//
//    menuButton->onHoverEnter = [menuRect]() {
//        menuRect->color = point3d(0.5f, 0.5f, 0.5f);
//        menuRect->opacity = 1.0f;
//        };
//
//    menuButton->onHoverExit = [menuRect]() {
//        menuRect->color = point3d(0.3f, 0.3f, 0.3f);
//        menuRect->opacity = 0.7f;
//        };*/
//
//    // Текст на кнопке меню
//    Entity* menuText = m_World->entityStorage->CreateEntity("MenuBtnText", menuBtn);
//    Transform2D* menuTextTr = menuText->AddComponent<Transform2D>();
//    menuTextTr->anchorPoint = point3d(0, 0, 0);
//    menuTextTr->ratio = ScreenAspectRatio::XY;
//    menuTextTr->position = point3d(-0.12f, -0.54f, 0.01f);
//
//    TextLabel* menuLabel = menuText->AddComponent<TextLabel>();
//    menuLabel->textW = L"ГЛАВНОЕ МЕНЮ [ESC]";
//    menuLabel->fontFamilyW = L"Impact";
//    menuLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
//    menuLabel->fontWeight = 500;
//    menuLabel->fontSizePx = 22;
//    menuLabel->fontScale = 1.0f;
//    menuLabel->color = point3d(0.9f, 0.9f, 0.9f);
//
//    // Анимация появления (мерцание текста)
//    Entity* pulseEffect = m_World->entityStorage->CreateEntity("PulseEffect", m_GameOverUI);
//    Transform2D* pulseTr = pulseEffect->AddComponent<Transform2D>();
//    pulseTr->anchorPoint = point3d(0, 0, 0);
//    pulseTr->ratio = ScreenAspectRatio::XY;
//    pulseTr->position = point3d(0, 0, -0.2f);
//    pulseTr->scale = point3d(1.0f, 1.0f, 0);
//
//    Rect* pulseRect = pulseEffect->AddComponent<Rect>();
//    pulseRect->color = point3d(1.0f, 0.0f, 0.0f);
//    pulseRect->opacity = 0.0f;
//    //pulseRect->isFilled = true;
//
//    m_Timer = 0.0f;
//    m_IsActive = true;
//}
//
//void GameOverState::Update()
//{
//    if (!m_IsActive) return;
//
//    m_Timer += 1.0f / 60.0f;
//
//    // Анимация пульсации фона
//    Entity* pulseEffect = m_World->entityStorage->GetEntityByName("PulseEffect");
//    if (pulseEffect)
//    {
//        Rect* pulseRect = pulseEffect->GetComponent<Rect>();
//        if (pulseRect)
//        {
//            // Медленная пульсация красного
//            float pulse = 0.05f + 0.03f * sin(m_Timer * 2.0f);
//            pulseRect->opacity = pulse;
//
//            // Увеличиваем масштаб для эффекта расширения
//            Transform2D* pulseTr = pulseEffect->GetComponent<Transform2D>();
//            if (pulseTr)
//            {
//                float scale = 1.0f + 0.02f * sin(m_Timer * 2.0f);
//                pulseTr->scale = point3d(scale, scale, 0);
//            }
//        }
//    }
//
//    // Анимация основного текста (лёгкое покачивание)
//    Entity* mainText = m_World->entityStorage->GetEntityByName("GameOverMainText");
//    if (mainText)
//    {
//        Transform2D* textTr = mainText->GetComponent<Transform2D>();
//        if (textTr)
//        {
//            float wobble = 0.003f * sin(m_Timer * 3.0f);
//            textTr->position.y = -0.05f + wobble;
//        }
//    }
//
//    // Анимация иконки черепа (вращение)
//    Entity* skullIcon = m_World->entityStorage->GetEntityByName("SkullIcon");
//    if (skullIcon)
//    {
//        Transform2D* skullTr = skullIcon->GetComponent<Transform2D>();
//        if (skullTr)
//        {
//            float rotation = 5.0f * sin(m_Timer * 1.5f);
//            //skullTr->rotation.z = rotation;
//
//            float scale = 1.5f + 0.1f * sin(m_Timer * 3.0f);
//            skullTr->scale = point3d(scale, scale, 0);
//        }
//    }
//
//    // Блокировка ввода на первую секунду (чтобы случайно не нажать)
//    if (m_Timer > 1.0f)
//    {
//        // Проверка клавиш
//        if (input::IsKeyPressed(VK_RETURN) || input::IsKeyPressed(VK_SPACE))
//        {
//            m_Manager->RestartGame();
//        }
//        else if (input::IsKeyPressed(VK_ESCAPE))
//        {
//            m_Manager->SwitchToMainMenu();
//        }
//
//        // Активируем кнопки после задержки
//        Entity* restartBtn = m_World->entityStorage->GetEntityByName("RestartButton");
//        if (restartBtn)
//        {
//            restartBtn->SetActive(true);
//        }
//
//        Entity* menuBtn = m_World->entityStorage->GetEntityByName("MenuButton");
//        if (menuBtn)
//        {
//            menuBtn->SetActive(true);
//        }
//    }
//    else
//    {
//        // Блокируем кнопки на время задержки
//        Entity* restartBtn = m_World->entityStorage->GetEntityByName("RestartButton");
//        if (restartBtn)
//        {
//            restartBtn->SetActive(false);
//        }
//
//        Entity* menuBtn = m_World->entityStorage->GetEntityByName("MenuButton");
//        if (menuBtn)
//        {
//            menuBtn->SetActive(false);
//        }
//    }
//
//    // Автоматический переход в главное меню через 30 секунд бездействия
//    if (m_Timer > 30.0f)
//    {
//        m_Manager->SwitchToMainMenu();
//    }
//}
//
//void GameOverState::Render()
//{
//    if (!m_IsActive) return;
//
//    // Получаем размеры окна через LevelManager
//    WindowClass* window = m_Manager->GetWindow();
//
//    ConstBuf::frame.aspect = XMFLOAT4{
//        float(window->aspect),
//        float(window->iaspect),
//        float(window->width),
//        float(window->height)
//    };
//
//    // Рендерим только UI (игровой мир уже не рендерится)
//    m_World->UpdateRender();
//
//    // Рендерим курсор
//    MouseClass* mouse = m_Manager->GetMouse();
//    if (mouse)
//    {
//        mouse->RenderCursor();
//    }
//}
//
//void GameOverState::Exit()
//{
//    if (m_GameOverUI)
//    {
//        m_GameOverUI->SetActive(false);
//        // Можно полностью удалить: m_World->entityStorage->DestroyEntity(m_GameOverUI);
//        m_GameOverUI = nullptr;
//    }
//
//    m_Timer = 0.0f;
//    m_IsActive = false;
//}