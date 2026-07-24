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
    void RemoveButton(size_t index);

    float CalculatePosition(size_t index, int depth);

    void SetPosition(Entity* button, float position);
    void UpdateText(const TreeItem& item);
    void SetExpandable(const TreeItem& item);
};

#endif