#pragma once
#include "GameState.h"
#include "../Engine/ECS_Base/world.h"
#include "../Engine/Window/windowclass.h"
#include "../Engine/Mouse/mouseclass.h"

class MainMenuState : public GameState
{
public:
    MainMenuState(LevelManagerClass* manager);
    ~MainMenuState();

    void Enter() override;
    void Update() override;
    
};