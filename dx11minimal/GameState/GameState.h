// GameState.h
#pragma once

// Forward declarations вместо include
class LevelManagerClass;
class World;
class WindowClass;
class MouseClass;
class Entity;

class GameState
{
protected:
    LevelManagerClass* m_Manager;
    World* m_World;
    WindowClass* m_Window;
    MouseClass* m_Mouse;
    Entity* m_MenuRoot;
    bool m_IsActive;

public:
    GameState(LevelManagerClass* manager)
        : m_Manager(manager)
        , m_World(nullptr)
        , m_Window(nullptr)
        , m_Mouse(nullptr)
        , m_MenuRoot(nullptr)
        , m_IsActive(false)
    {
    }

    virtual ~GameState() = default;

    virtual void Enter() = 0;
    virtual void Exit();
    virtual void Update() = 0;

    void SetWorld(World* world) { m_World = world; }
    void SetWindow(WindowClass* window) { m_Window = window; }
    void SetMouse(MouseClass* mouse) { m_Mouse = mouse; }

    bool IsActive() const { return m_IsActive; }
};