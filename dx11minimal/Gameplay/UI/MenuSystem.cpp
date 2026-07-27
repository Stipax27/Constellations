#include "MenuSystem.h"
#include "MenuComponents.h"
#include "../../Engine/BasicComponents/Transform2D.h"
#include "../../Engine/UI/Rect.h"
#include "../../Engine/UI/Button.h"
#include "../../Engine/UI/Text/TextLabel.h"
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

    // Защита от ghost click — игнорируем ввод N кадров после переключения меню
    if (m_ignoreInputFrames > 0) {
        m_ignoreInputFrames--;
        return;
    }

    std::vector<std::function<void()>> clickQueue;

    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<MenuButton>();
    for (Entity* entity : entities) {
        if (!IsEntityValid(entity)) continue;
        MenuButton* menuButton = entity->GetComponent<MenuButton>();
        if (!menuButton) continue;
        Button* button = entity->GetComponent<Button>();
        if (!button) continue;

        if (button->isClicked && !menuButton->wasClicked) {
            menuButton->wasClicked = true;
            if (menuButton->onClick) clickQueue.push_back(menuButton->onClick);
        }
        else if (!button->isClicked) {
            menuButton->wasClicked = false;
        }
    }

    // Выполняем коллбеки ПОСЛЕ итерации
    for (auto& fn : clickQueue) fn();
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
    b->color = point3d(0.15f, 0.15f, 0.25f);
    b->clickColor = point3d(0.3f, 0.4f, 0.6f);
    b->opacity = 1.0f;
    // Уменьшаем cornerRadius — при scale.y = 0.07 радиус 0.1 даёт слишком сильное скругление
    b->cornerRadius = 0.04f;  // Было 0.1f — уменьшили для чёткости
    b->cornerType = CornerType::Smooth;

    TextLabel* txt = e->AddComponent<TextLabel>();
    txt->textW = std::wstring(text.begin(), text.end());
    txt->color = point3d(1.0f, 1.0f, 1.0f);
    txt->fontSizePx = 22;  // Чуть меньше — было 24
    txt->centered = true;
    txt->fontFamilyW = L"Arial";
    // Сдвигаем текст вверх, чтобы он был по центру кнопки
    // Твой UITextSystem рендерит от baseline, так что нужен отрицательный offset
    txt->verticalOffset = -4.0f;  // Подбирай экспериментально: -2, -4, -6, -8

    MenuButton* mb = e->AddComponent<MenuButton>();
    mb->onClick = onClick;
    mb->wasClicked = false;
    return e;
}

Entity* MenuSystem::CreateMainMenu() {
    Entity* menu = m_entityStorage->CreateEntity("MainMenu", nullptr);
    menu->AddComponent<MenuElement>()->isMainMenu = true;

    const float spacing = 0.13f;  // Увеличили — было 0.11f, кнопки перекрывались
    const float startY = 0.26f;   // Чуть выше, чтобы всё поместилось

    Entity* btnStart = CreateButton("Start",
        point3d(0.0f, startY, 0),
        point3d(0.25f, 0.07f, 0),  // Чуть ниже — было 0.08f
        [this]() { OnStartClicked(); });
    if (btnStart) btnStart->SetParent(menu);

    Entity* btnLoad = CreateButton("Load Game",
        point3d(0.0f, startY - spacing, 0),
        point3d(0.25f, 0.07f, 0),
        [this]() { OnLoadGameClicked(); });
    if (btnLoad) btnLoad->SetParent(menu);

    Entity* btnSave = CreateButton("Save Game",
        point3d(0.0f, startY - spacing * 2.0f, 0),
        point3d(0.25f, 0.07f, 0),
        [this]() { OnSaveGameClicked(); });
    if (btnSave) btnSave->SetParent(menu);

    Entity* btnSettings = CreateButton("Settings",
        point3d(0.0f, startY - spacing * 3.0f, 0),
        point3d(0.25f, 0.07f, 0),
        [this]() { OnSettingsClicked(); });
    if (btnSettings) btnSettings->SetParent(menu);

    Entity* btnExit = CreateButton("Exit",
        point3d(0.0f, startY - spacing * 4.0f, 0),
        point3d(0.25f, 0.07f, 0),
        [this]() { OnExitClicked(); });
    if (btnExit) btnExit->SetParent(menu);

    return menu;
}

Entity* MenuSystem::CreatePauseMenu() {
    Entity* menu = m_entityStorage->CreateEntity("PauseMenu", nullptr);
    menu->AddComponent<MenuElement>()->isPauseMenu = true;

    const float spacing = 0.13f;

    Entity* btnResume = CreateButton("Resume",
        point3d(0.0f, 0.12f, 0),
        point3d(0.22f, 0.07f, 0),
        [this]() { OnResumeClicked(); });
    if (btnResume) btnResume->SetParent(menu);

    Entity* btnSettings = CreateButton("Settings",
        point3d(0.0f, 0.12f - spacing, 0),
        point3d(0.22f, 0.07f, 0),
        [this]() { OnSettingsClicked(); });
    if (btnSettings) btnSettings->SetParent(menu);

    Entity* btnMainMenu = CreateButton("Main Menu",
        point3d(0.0f, 0.12f - spacing * 2.0f, 0),
        point3d(0.22f, 0.07f, 0),
        [this]() { OnMainMenuClicked(); });
    if (btnMainMenu) btnMainMenu->SetParent(menu);

    return menu;
}

void MenuSystem::OnStartClicked() { HideAllMenus(); }
void MenuSystem::OnResumeClicked() { HideAllMenus(); }
void MenuSystem::OnMainMenuClicked() { ShowMainMenu(); }
void MenuSystem::OnExitClicked() { PostQuitMessage(0); }

void MenuSystem::OnSettingsClicked() {
    // TODO: Открыть меню настроек
    // Например: ShowSettingsMenu();
}

void MenuSystem::OnSaveGameClicked() {
    // TODO: Сохранить игру
    // Например: SaveSystem::Save();
}

void MenuSystem::OnLoadGameClicked() {
    // TODO: Загрузить игру
    // Например: SaveSystem::Load();
}

void MenuSystem::ShowMainMenu() {
    ClearAllMenus();
    Entity* menu = m_entityStorage->GetEntityByName("MainMenu");
    if (!menu || !IsEntityValid(menu)) menu = CreateMainMenu();
    menu->SetActive(true);
    m_activeMenu = menu;

    if (m_overlayEntity) {
        m_overlayEntity->SetActive(true);
        Rect* overlayRect = m_overlayEntity->GetComponent<Rect>();
        if (overlayRect) overlayRect->opacity = 1.0f;
    }

    Entity* WorldFolder = m_entityStorage->GetEntityByName("World");
    if (WorldFolder) WorldFolder->SetTimeScale(0);
    Entity* Player = m_entityStorage->GetEntityByName("Player");
    if (Player) Player->SetTimeScale(0);
    mouse->state = MouseState::Free;

    // Защита от ghost click
    m_ignoreInputFrames = 5;
}

void MenuSystem::ShowPauseMenu() {
    ClearAllMenus();
    Entity* menu = m_entityStorage->GetEntityByName("PauseMenu");
    if (!menu || !IsEntityValid(menu)) menu = CreatePauseMenu();
    menu->SetActive(true);
    m_activeMenu = menu;

    if (m_overlayEntity) {
        m_overlayEntity->SetActive(true);
        Rect* overlayRect = m_overlayEntity->GetComponent<Rect>();
        if (overlayRect) overlayRect->opacity = 0.7f;
    }

    Entity* WorldFolder = m_entityStorage->GetEntityByName("World");
    if (WorldFolder) WorldFolder->SetTimeScale(0);
    Entity* Player = m_entityStorage->GetEntityByName("Player");
    if (Player) Player->SetTimeScale(0);
    mouse->state = MouseState::Free;

    // Защита от ghost click
    m_ignoreInputFrames = 5;
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
    m_activeMenu = nullptr;
}