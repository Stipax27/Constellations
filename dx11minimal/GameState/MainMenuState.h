
#pragma once
#include "../Engine/ECS_Base/world.h"
#include "../Engine/Window/windowclass.h"
#include "../Engine/Mouse/mouseclass.h"

class LevelManagerClass;

class MainMenuState
{
private:
    LevelManagerClass* m_Manager;
    World* m_World;
    WindowClass* m_Window;
    MouseClass* m_Mouse;
    Entity* m_MenuRoot;
    bool m_IsActive;

public:
    MainMenuState(LevelManagerClass* manager);
    ~MainMenuState();

    void Enter();
    void Exit();
    void Update();
    void Render();

    bool IsActive() const { return m_IsActive; }
};