// GameStates/PauseState.h
#pragma once
#include "../Engine/ECS_Base/world.h"

class LevelManagerClass;

class PauseState
{
private:
    LevelManagerClass* m_Manager;
    World* m_World;
    Entity* m_PauseMenu;
    bool m_IsActive;

public:
    PauseState(LevelManagerClass* manager);
    ~PauseState();

    void Enter();
    void Exit();
    void Update();
    void Render();

    bool IsActive() const { return m_IsActive; }
};
