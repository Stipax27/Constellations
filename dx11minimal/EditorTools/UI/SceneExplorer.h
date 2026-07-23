#ifndef _SCENE_EXPLORER_H_
#define _SCENE_EXPLORER_H_

#include "TreeViewModel.h"


class SceneExplorer : public ISingleton
{
public:
    void Initialize() override;
    void Update();

    void OnToggleExpand(size_t index);

private:
    EntityStorage* entityStorage;
    Entity* explorerWindow;

    TreeViewModel model;
    std::vector<Entity*> uiButtons; // ваши кнопки

private:
    void Render();

    void UpdateUI();

    void CreateExplorerWindow();
    Entity* CreateButton(const TreeItem& item);
};

#endif