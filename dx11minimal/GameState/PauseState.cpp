// GameStates/PauseState.cpp
#include "PauseState.h"
#include "..\\LevelManagerClass.h"

PauseState::PauseState(LevelManagerClass* manager)
    : m_Manager(manager)
    , m_World(nullptr)
    , m_PauseMenu(nullptr)
    , m_IsActive(false)
{
   
}

PauseState::~PauseState()
{
    Exit();
}

void PauseState::Enter()
{
    if (m_IsActive) return;

    m_PauseMenu = m_World->entityStorage->CreateEntity("PauseMenu");

    // Полупрозрачный фон
    Entity* bg = m_World->entityStorage->CreateEntity("PauseBG", m_PauseMenu);
    Transform2D* bgTr = bg->AddComponent<Transform2D>();
    bgTr->anchorPoint = point3d(0, 0, 0);
    bgTr->ratio = ScreenAspectRatio::XY;
    bgTr->position = point3d(-0.5f, -0.5f, 0);
    bgTr->scale = point3d(1, 1, 0);

    Rect* bgRect = bg->AddComponent<Rect>();
    bgRect->color = point3d(0, 0, 0);
    bgRect->opacity = 0.7f;

    // Заголовок "ПАУЗА"
    Entity* title = m_World->entityStorage->CreateEntity("PauseTitle", m_PauseMenu);
    Transform2D* titleTr = title->AddComponent<Transform2D>();
    titleTr->anchorPoint = point3d(0, 0, 0);
    titleTr->ratio = ScreenAspectRatio::XY;
    titleTr->position = point3d(-0.1f, 0.2f, 0);

    TextLabel* titleText = title->AddComponent<TextLabel>();
    titleText->textW = L"ПАУЗА";
    titleText->fontFamilyW = L"Impact";
    titleText->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    titleText->fontWeight = 900;
    titleText->fontSizePx = 70;
    titleText->color = point3d(1, 1, 1);

    m_IsActive = true;
}

void PauseState::Update()
{
    if (!m_IsActive) return;

    if (input::IsKeyPressed(VK_ESCAPE))
    {
        //m_Manager->ResumeFromPause();
    }

    if (input::IsKeyPressed('M'))
    {
        //m_Manager->SwitchToMainMenu();
    }
}

void PauseState::Render()
{
    // Рендер осуществляется в LevelManagerClass::Frame()
}

void PauseState::Exit()
{
    if (m_PauseMenu)
    {
        m_PauseMenu->SetActive(false);
        m_PauseMenu = nullptr;
    }
    m_IsActive = false;
}