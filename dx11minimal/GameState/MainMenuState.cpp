#include "MainMenuState.h"
#include "..\\LevelManagerClass.h"

MainMenuState::MainMenuState(LevelManagerClass* manager)
    : m_Manager(manager)
    , m_World(nullptr)
    , m_Window(nullptr)
    , m_Mouse(nullptr)
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
    if (m_IsActive || !m_World) return;  // не входим повторно или без мира

    // Создаём корневой контейнер меню
    m_MenuRoot = m_World->entityStorage->CreateEntity("MainMenu");

    // Заголовок
    Entity* title = m_World->entityStorage->CreateEntity("Title", m_MenuRoot);
    Transform2D* t = title->AddComponent<Transform2D>();
    t->anchorPoint = point3d(0, 0, 0);
    t->ratio = ScreenAspectRatio::XY;
    t->position = point3d(-0.35f, 0.3f, 0);

    TextLabel* label = title->AddComponent<TextLabel>();
    label->textW = L"КОСМИЧЕСКАЯ ОДИССЕЯ";
    label->fontFamilyW = L"Impact";
    label->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    label->fontWeight = 900;
    label->fontSizePx = 80;
    label->color = point3d(0.8f, 0.6f, 1.0f);

    // Кнопка "Начать игру"
    Entity* startBtn = m_World->entityStorage->CreateEntity("StartBtn", m_MenuRoot);
    Transform2D* btnTr = startBtn->AddComponent<Transform2D>();
    btnTr->anchorPoint = point3d(0, 0, 0);
    btnTr->ratio = ScreenAspectRatio::XY;
    btnTr->position = point3d(-0.15f, -0.1f, 0);
    btnTr->scale = point3d(0.3f, 0.07f, 0);

    Rect* rect = startBtn->AddComponent<Rect>();
    rect->color = point3d(0.2f, 0.4f, 0.8f);
    rect->cornerRadius = 0.05f;

    Entity* btnText = m_World->entityStorage->CreateEntity("StartText", startBtn);
    Transform2D* textTr = btnText->AddComponent<Transform2D>();
    textTr->anchorPoint = point3d(0, 0, 0);
    textTr->ratio = ScreenAspectRatio::XY;
    textTr->position = point3d(-0.08f, 0.01f, 0);

    TextLabel* btnLabel = btnText->AddComponent<TextLabel>();
    btnLabel->textW = L"НАЧАТЬ ИГРУ";
    btnLabel->fontFamilyW = L"Impact";
    btnLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    btnLabel->fontSizePx = 32;
    btnLabel->color = point3d(1, 1, 1);

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

void MainMenuState::Exit()
{
    if (m_MenuRoot)
    {
        m_MenuRoot->SetActive(false);
        m_MenuRoot = nullptr;   // обнуляем, чтобы избежать повторного использования
    }
    m_IsActive = false;
}

void MainMenuState::Update()
{
    if (!m_IsActive) return;

    // Обработка ввода
    if (input::IsKeyPressed(VK_RETURN) || input::IsKeyPressed(VK_SPACE))
    {
        //MainMenuState::m_Manager->SwitchToGameState();
    }
    if (input::IsKeyPressed(VK_ESCAPE))
    {
        PostQuitMessage(0);
    }
}

void MainMenuState::Render()
{
    if (!m_IsActive || !m_World) return;

    ConstBuf::frame.aspect = XMFLOAT4{
        float(m_Window->aspect),
        float(m_Window->iaspect),
        float(m_Window->width),
        float(m_Window->height)
    };

    m_World->UpdateRender();
    if (m_Mouse)
        m_Mouse->RenderCursor();
}