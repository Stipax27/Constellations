#pragma once
#include "../../Engine/ECS_Base/component.h"
#include <functional>

struct MenuElement : Component {
    bool isMainMenu = false;
    bool isPauseMenu = false;
    bool isVisible = true;
};

struct MenuButton : Component {
    std::function<void()> onClick;
    bool wasClicked = false;
};
