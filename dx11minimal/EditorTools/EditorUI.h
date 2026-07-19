#ifndef _EDITOR_UI_H_
#define _EDITOR_UI_H_

#include "DragController.h"


#define EXPLORER_WIDTH 0.2f
#define EXPLORER_ITEM_HEIGHT 0.03f


struct ExplorerItem
{
	Entity* entity;
	std::vector<ExplorerItem> children;

	ExplorerItem() = default;

	ExplorerItem(Entity* entity)
		: entity(entity)
	{
	};
};


class EditorUI : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	DragController* dragController;
	EntityStorage* entityStorage;

	Entity* explorerWindow;

private:
	void InitExplorer();
	void UpdateEntityList();
	void CreateItemButton(const ExplorerItem&, int pos);
	ExplorerItem NewItem(Entity*);
	void SortItemsAlphabetically(std::vector<ExplorerItem>&);
};

#endif
