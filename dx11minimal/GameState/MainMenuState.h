
#pragma once
#include "../Engine/ECS_Base/world.h"
#include "../Engine/Window/windowclass.h"

class LevelManagerClass; // Предварительное объявление

class MainMenuState
{
private:
    LevelManagerClass* m_Manager;
    World* m_World;           // Храним указатель на мир
    WindowClass* m_Window;    // Храним указатель на окно
    Entity* m_MenuRoot;
    bool m_IsActive;

public:
    // Передаём World* и WindowClass* напрямую
    MainMenuState(World* world, WindowClass* window);
    ~MainMenuState();

    void Enter();
    void Exit();
    void Update();
    void Render();

    bool IsActive() const { return m_IsActive; }
};
