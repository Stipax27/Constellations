#include "MainMenuState.h"
#include "..\..\LevelManagerClass.h"

MainMenuState::MainMenuState(World* world, WindowClass* window)
    : m_Manager(nullptr)
    , m_World(world)          // Сохраняем переданные указатели
    , m_Window(window)
    , m_MenuRoot(nullptr)
    , m_IsActive(false)
{
}

MainMenuState::~MainMenuState()
{
    Exit();
}

void MainMenuState::Enter()
{
    if (m_IsActive) return;

    m_MenuRoot = m_World->entityStorage->CreateEntity("MainMenu");

    // Заголовок
    Entity* title = m_World->entityStorage->CreateEntity("Title", m_MenuRoot);
    Transform2D* titleTr = title->AddComponent<Transform2D>();
    titleTr->anchorPoint = point3d(0, 0, 0);
    titleTr->ratio = ScreenAspectRatio::XY;
    titleTr->position = point3d(-0.35f, 0.3f, 0);

    TextLabel* titleText = title->AddComponent<TextLabel>();
    titleText->textW = L"The 13th Sign";
    titleText->fontFamilyW = L"Impact";
    titleText->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    titleText->fontWeight = 900;
    titleText->fontSizePx = 80;
    titleText->color = point3d(0.8f, 0.6f, 1.0f);

    // Кнопка "Начать"
    Entity* startBtn = m_World->entityStorage->CreateEntity("StartBtn", m_MenuRoot);
    Transform2D* btnTr = startBtn->AddComponent<Transform2D>();
    btnTr->anchorPoint = point3d(0, 0, 0);
    btnTr->ratio = ScreenAspectRatio::XY;
    btnTr->position = point3d(-0.15f, -0.1f, 0);
    btnTr->scale = point3d(0.3f, 0.07f, 0);

    Rect* btnRect = startBtn->AddComponent<Rect>();
    btnRect->color = point3d(0.2f, 0.4f, 0.8f);
    btnRect->cornerRadius = 0.05f;

    Entity* btnText = m_World->entityStorage->CreateEntity("StartText", startBtn);
    Transform2D* textTr = btnText->AddComponent<Transform2D>();
    textTr->anchorPoint = point3d(0, 0, 0);
    textTr->ratio = ScreenAspectRatio::XY;
    textTr->position = point3d(-0.08f, 0.01f, 0);

    TextLabel* label = btnText->AddComponent<TextLabel>();
    label->textW = L"НАЧАТЬ ИГРУ";
    label->fontFamilyW = L"Impact";
    label->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    label->fontSizePx = 32;
    label->color = point3d(1, 1, 1);

    // Кнопка "Выход"
    Entity* exitBtn = m_World->entityStorage->CreateEntity("ExitBtn", m_MenuRoot);
    Transform2D* exitTr = exitBtn->AddComponent<Transform2D>();
    exitTr->anchorPoint = point3d(0, 0, 0);
    exitTr->ratio = ScreenAspectRatio::XY;
    exitTr->position = point3d(-0.15f, -0.25f, 0);
    exitTr->scale = point3d(0.3f, 0.07f, 0);

    Rect* exitRect = exitBtn->AddComponent<Rect>();
    exitRect->color = point3d(0.2f, 0.4f, 0.8f);
    exitRect->cornerRadius = 0.05f;

    Entity* exitText = m_World->entityStorage->CreateEntity("ExitText", exitBtn);
    Transform2D* exitTextTr = exitText->AddComponent<Transform2D>();
    exitTextTr->anchorPoint = point3d(0, 0, 0);
    exitTextTr->ratio = ScreenAspectRatio::XY;
    exitTextTr->position = point3d(-0.06f, 0.01f, 0);

    TextLabel* exitLabel = exitText->AddComponent<TextLabel>();
    exitLabel->textW = L"ВЫХОД";
    exitLabel->fontFamilyW = L"Impact";
    exitLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    exitLabel->fontSizePx = 32;
    exitLabel->color = point3d(1, 1, 1);

    // Фон меню
    Entity* bg = m_World->entityStorage->CreateEntity("MenuBG", m_MenuRoot);
    Transform2D* bgTr = bg->AddComponent<Transform2D>();
    bgTr->anchorPoint = point3d(0, 0, 0);
    bgTr->ratio = ScreenAspectRatio::XY;
    bgTr->position = point3d(-0.5f, -0.5f, -0.1f);
    bgTr->scale = point3d(1.0f, 1.0f, 0);

    Rect* bgRect = bg->AddComponent<Rect>();
    bgRect->color = point3d(0.05f, 0.05f, 0.15f);
    bgRect->opacity = 0.8f;

    m_IsActive = true;
}

void MainMenuState::Update()
{
    if (!m_IsActive) return;

    // Проверка ввода
    if (input::IsKeyPressed(VK_RETURN) || input::IsKeyPressed(VK_SPACE))
    {
        m_Manager->SwitchToGameplay();
    }

    if (input::IsKeyPressed(VK_ESCAPE))
    {
        PostQuitMessage(0);
    }
}

void MainMenuState::Render()
{
    if (!m_IsActive) return;

    // Используем m_Window напрямую
    ConstBuf::frame.aspect = XMFLOAT4{
        float(m_Window->aspect),
        float(m_Window->iaspect),
        float(m_Window->width),
        float(m_Window->height)
    };

    m_World->UpdateRender();  // Используем m_World напрямую
}

void MainMenuState::Exit()
{
    if (m_MenuRoot)
    {
        m_MenuRoot->SetActive(false);
        m_MenuRoot = nullptr;
    }
    m_IsActive = false;
}