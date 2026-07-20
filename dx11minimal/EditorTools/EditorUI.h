#ifndef _EDITOR_UI_H_
#define _EDITOR_UI_H_

#include "DragController.h"


#define EXPLORER_WIDTH 0.2f
#define EXPLORER_ITEM_HEIGHT 0.025f
#define EXPLORER_ITEM_OFFSET 0.01f


struct ExplorerItem
{
	Entity* entity = nullptr;
	Entity* button = nullptr;
	std::vector<ExplorerItem> children;

	bool opened = false;

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

	std::vector<ExplorerItem> itemList;

private:
	void InitExplorer();
	void UpdateEntityList();
	void UpdateItems();
	void ItemsOfList(std::vector<ExplorerItem>& list, int& count);
	void CreateItemButton(ExplorerItem&, int pos);
	ExplorerItem NewItem(Entity*);
	void SortItemsAlphabetically(std::vector<ExplorerItem>&);
};

#endif
