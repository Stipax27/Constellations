#include "MenuSystem.h"
#include "MenuComponents.h"
#include "../../Engine/BasicComponents/Transform2D.h"
#include "../../Engine/UI/Rect.h"
#include "../../Engine/UI/Button.h"
#include "../../Engine/UI/Text/TextLabel.h" // <--- ДОБАВЛЕН INCLUDE ДЛЯ ТЕКСТА
#include "../../Engine/Lib/timer.h"
#include "../../Engine/dx11.h"
#include "../../Engine/Lib/logging.h"

MenuSystem::MenuSystem() = default;
MenuSystem::~MenuSystem() { Shutdown(); }

void MenuSystem::Initialize() {
    if (m_isInitialized) return;
    m_entityStorage = Singleton::GetInstance<EntityStorage>();
    mouse = Singleton::GetInstance<MouseClass>();
    m_overlayEntity = CreateOverlay();
    m_overlayEntity->SetActive(false);
    m_isInitialized = true;
    ShowMainMenu();
}

void MenuSystem::Shutdown() {
    if (m_overlayEntity) { m_overlayEntity->Destroy(); m_overlayEntity = nullptr; }
    ClearAllMenus();
    m_isInitialized = false;
}

void MenuSystem::Update(EntityStorage& entityStorage, float deltaTime) {
    if (!m_activeMenu) return;

    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<MenuButton>();
    for (Entity* entity : entities) {
        if (!IsEntityValid(entity)) continue;
        MenuButton* menuButton = entity->GetComponent<MenuButton>();
        if (!menuButton) continue;
        Button* button = entity->GetComponent<Button>();
        if (!button) continue;

        if (button->isClicked && !menuButton->wasClicked) {
            menuButton->wasClicked = true;
            if (menuButton->onClick) menuButton->onClick();
        }
        else if (!button->isClicked) {
            menuButton->wasClicked = false;
        }
    }
}

// ===== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ =====
Entity* MenuSystem::CreateOverlay() {
    Entity* overlay = m_entityStorage->CreateEntity("MenuOverlay", nullptr);
    Transform2D* t = overlay->AddComponent<Transform2D>();
    t->position = point3d(0, 0, 0); t->scale = point3d(1, 1, 0); t->anchorPoint = point3d(0, 0, 0); t->ratio = ScreenAspectRatio::XY;
    Rect* r = overlay->AddComponent<Rect>();
    r->color = point3d(0, 0, 0); r->opacity = 0.7f;
    return overlay;
}

Entity* MenuSystem::CreateButton(const std::string& text, const point3d& pos, const point3d& scale,
    std::function<void()> onClick) {

    Entity* e = m_entityStorage->CreateEntity("MenuButton", nullptr);

    Transform2D* t = e->AddComponent<Transform2D>();
    t->position = pos;
    t->scale = scale;
    t->anchorPoint = point3d(0.0f, 0.0f, 0);
    t->ratio = ScreenAspectRatio::XY;

    Button* b = e->AddComponent<Button>();
    // Стильные цвета: обычный - темно-синий, при клике - светло-серый/голубой
    b->color = point3d(0.15f, 0.15f, 0.25f);
    b->clickColor = point3d(0.3f, 0.4f, 0.6f);
    b->opacity = 1.0f;
    b->cornerRadius = 0.1f; // Включаем красивые скругленные углы
    b->cornerType = CornerType::Smooth;

    // --- ДОБАВЛЯЕМ ТЕКСТ ---
    TextLabel* txt = e->AddComponent<TextLabel>();
    // Конвертируем std::string в std::wstring для твоего TextLabel
    txt->textW = std::wstring(text.begin(), text.end());
    txt->color = point3d(1.0f, 1.0f, 1.0f); // Белый текст
    txt->fontSizePx = 24; // Размер шрифта (можешь покрутить, если мало/велико)
    txt->centered = true; // Выравнивание по центру кнопки
    txt->fontFamilyW = L"Arial"; // Можно оставить дефолтный, но так нагляднее

    MenuButton* mb = e->AddComponent<MenuButton>();
    mb->onClick = onClick;
    return e;
}

Entity* MenuSystem::CreateMainMenu() {
    Entity* menu = m_entityStorage->CreateEntity("MainMenu", nullptr);
    menu->AddComponent<MenuElement>()->isMainMenu = true;

    // Y = 0.1 - это ВЕРХ (Первая кнопка)
    Entity* btnStart = CreateButton("Start",
        point3d(0.0f, 0.1f, 0),
        point3d(0.2f, 0.08f, 0),
        [this]() { OnStartClicked(); });
    if (btnStart) btnStart->SetParent(menu);

    // Y = -0.1 - это НИЗ (Вторая кнопка)
    Entity* btnExit = CreateButton("Exit",
        point3d(0.0f, -0.1f, 0),
        point3d(0.2f, 0.08f, 0),
        [this]() { OnExitClicked(); });
    if (btnExit) btnExit->SetParent(menu);

    return menu;
}

Entity* MenuSystem::CreatePauseMenu() {
    Entity* menu = m_entityStorage->CreateEntity("PauseMenu", nullptr);
    menu->AddComponent<MenuElement>()->isPauseMenu = true;

    // Y положительное - ВЕРХ (Продолжить)
    CreateButton("Resume",
        point3d(0.0f, 0.1f, 0),
        point3d(0.2f, 0.08f, 0),
        [this]() { OnResumeClicked(); })->SetParent(menu);

    // Y отрицательное - НИЗ (В главное меню)
    CreateButton("Main Menu",
        point3d(0.0f, -0.1f, 0),
        point3d(0.2f, 0.08f, 0),
        [this]() { OnMainMenuClicked(); })->SetParent(menu);

    return menu;
}

void MenuSystem::OnStartClicked() { HideAllMenus(); }
void MenuSystem::OnResumeClicked() { HideAllMenus(); }
void MenuSystem::OnMainMenuClicked() { ShowMainMenu(); }
void MenuSystem::OnExitClicked() { PostQuitMessage(0); }

void MenuSystem::ShowMainMenu() {
    ClearAllMenus();
    Entity* menu = m_entityStorage->GetEntityByName("MainMenu");
    if (!menu) menu = CreateMainMenu();
    menu->SetActive(true);
    m_activeMenu = menu;

    if (m_overlayEntity) {
        m_overlayEntity->SetActive(true);
        Rect* overlayRect = m_overlayEntity->GetComponent<Rect>();
        if (overlayRect) overlayRect->opacity = 1.0f; // <--- ГЛАВНОЕ МЕНЮ: 100% ЧЕРНЫЙ ФОН
    }

    Entity* WorldFolder = m_entityStorage->GetEntityByName("World");
    if (WorldFolder) WorldFolder->SetTimeScale(0);
    Entity* Player = m_entityStorage->GetEntityByName("Player");
    if (Player) Player->SetTimeScale(0);
    mouse->state = MouseState::Free;
}

void MenuSystem::ShowPauseMenu() {
    ClearAllMenus();
    Entity* menu = m_entityStorage->GetEntityByName("PauseMenu");
    if (!menu) menu = CreatePauseMenu();
    menu->SetActive(true);
    m_activeMenu = menu;

    if (m_overlayEntity) {
        m_overlayEntity->SetActive(true);
        Rect* overlayRect = m_overlayEntity->GetComponent<Rect>();
        if (overlayRect) overlayRect->opacity = 0.7f; // <--- ПАУЗА: 70% ПОЛУПРОЗРАЧНЫЙ ФОН
    }

    Entity* WorldFolder = m_entityStorage->GetEntityByName("World");
    if (WorldFolder) WorldFolder->SetTimeScale(0);
    Entity* Player = m_entityStorage->GetEntityByName("Player");
    if (Player) Player->SetTimeScale(0);
    mouse->state = MouseState::Free;
}

void MenuSystem::HideAllMenus() {
    ClearAllMenus();
    if (m_overlayEntity) m_overlayEntity->SetActive(false);
    m_activeMenu = nullptr;

    Entity* WorldFolder = m_entityStorage->GetEntityByName("World");
    if (WorldFolder) WorldFolder->SetTimeScale(1);
    Entity* Player = m_entityStorage->GetEntityByName("Player");
    if (Player) Player->SetTimeScale(1);
    mouse->state = MouseState::Locked;
}

void MenuSystem::ClearAllMenus() {
    const std::vector<Entity*>& entities = m_entityStorage->GetEntitiesWithComponent<MenuElement>();
    for (Entity* e : entities) { if (IsEntityValid(e)) e->Destroy(); }
    
}