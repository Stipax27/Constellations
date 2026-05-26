// GameStates/GameOverState.h
#pragma once
#include "../Engine/ECS_Base/world.h"

class LevelManagerClass;

class GameOverState
{
private:
    LevelManagerClass* m_Manager;
    World* m_World;
    Entity* m_GameOverUI;
    bool m_IsActive;
    float m_Timer;

public:
    GameOverState(LevelManagerClass* manager);
    ~GameOverState();

    void Enter();
    void Exit();
    void Update();
    void Render();

    bool IsActive() const { return m_IsActive; }
};
