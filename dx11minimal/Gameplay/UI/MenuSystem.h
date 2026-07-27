#pragma once
#include "../../Engine/ECS_Base/system.h"
#include "../../Engine/ECS_Base/entityStorage.h"
#include "../../Engine/Mouse/mouseclass.h"
#include "../../Engine/Lib/singleton.h"
#include "../../Engine/Types/point3d.h"
#include <functional>
#include <string>

class MenuSystem : public System {
public:
    MenuSystem();
    virtual ~MenuSystem();

    void Initialize() override;
    void Shutdown() override;
    void Update(EntityStorage& entityStorage, float deltaTime) override;

    void ShowMainMenu();
    void ShowPauseMenu();
    void HideAllMenus();
    bool IsMenuVisible() const { return m_activeMenu != nullptr; }

private:
    EntityStorage* m_entityStorage = nullptr;
    MouseClass* mouse = nullptr;
    Entity* m_activeMenu = nullptr;
    Entity* m_overlayEntity = nullptr;
    bool m_isInitialized = false;

    // Защита от ghost click при переключении меню
    int m_ignoreInputFrames = 0;

    Entity* CreateOverlay();
    Entity* CreateMainMenu();
    Entity* CreatePauseMenu();
    Entity* CreateButton(const std::string& text,
        const point3d& position,
        const point3d& scale,
        std::function<void()> onClick);

    void OnStartClicked();
    void OnResumeClicked();
    void OnMainMenuClicked();
    void OnExitClicked();
    void OnSettingsClicked();
    void OnSaveGameClicked();
    void OnLoadGameClicked();
    void ClearAllMenus();
    void ResetButtonStates();
};
