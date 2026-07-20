#include "EditorUI.h"
#include "../Engine/Utils/componentutils.h"
#include "../ecsHeaders.h"

using namespace std;


void EditorUI::Initialize()
{
	entityStorage = Singleton::GetInstance<EntityStorage>();

	InitExplorer();
}


void EditorUI::Shutdown()
{
	if (entityStorage)
		entityStorage = 0;
}


void EditorUI::Update()
{
	for (ExplorerItem& item : itemList) {
		if (!IsEntityValid(item.button))
			continue;

		Button* button = item.button->GetComponent<Button>();
		if (button->isReleased) {

			item.opened = !item.opened;

			Entity* arrow = item.button->GetChildByName("Arrow", true);
			arrow->GetComponent<Transform2D>()->rotation = item.opened ? -PI / 2 : 0;

			UpdateItems();
		}
	}
}


void EditorUI::InitExplorer()
{
	explorerWindow = entityStorage->CreateEntity("Explorer");

	Transform2D* transform2D = explorerWindow->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->position = point3d(-1, 0, 0);
	transform2D->scale = point3d(EXPLORER_WIDTH, 1, 0);

	Rect* rect = explorerWindow->AddComponent<Rect>();
	rect->color = point3d(0.25f, 0.25f, 0.25f);
	rect->opacity = 0.75f;

	UpdateEntityList();
}


void EditorUI::UpdateEntityList()
{
	for (Entity* entity : entityStorage->entities) {
		if (entity->GetParent() == nullptr) {
			ExplorerItem item = NewItem(entity);
			itemList.push_back(item);
		}
	}
	SortItemsAlphabetically(itemList);

	int size = itemList.size();
	for (int i = 0; i < size; i++) {
		ExplorerItem& item = itemList[i];
		CreateItemButton(item, i);
	}
}


void EditorUI::UpdateItems()
{
	explorerWindow->ClearChildren();

	int count = 0;
	//ItemsOfList(itemList, count);
}


void EditorUI::ItemsOfList(vector<ExplorerItem>& list, int& count)
{
	for (ExplorerItem& item : list) {
		CreateItemButton(item, count);
		count++;

		if (item.opened) {
			ItemsOfList(item.children, count);
		}
	}
}


void EditorUI::CreateItemButton(ExplorerItem& item, int pos)
{
	// Button

	Entity* itemEntity = entityStorage->CreateEntity("Item", explorerWindow);

	Transform2D* transform2D = itemEntity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 1, 0);
	transform2D->position = point3d(0, 1.0f - (EXPLORER_ITEM_HEIGHT * 2 * pos + EXPLORER_ITEM_OFFSET * (pos + 1)), 0);
	transform2D->scale = point3d(0.95f, EXPLORER_ITEM_HEIGHT, 0);

	Button* button = itemEntity->AddComponent<Button>();
	button->color = point3d(0.4f, 0.4f, 0.4f);
	button->clickColor = point3d(0.5f, 0.5f, 0.5f);

	TextLabel* textLabel = itemEntity->AddComponent<TextLabel>();
	textLabel->textW = string_to_wstring(item.entity->name);
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 500;
	textLabel->fontSizePx = 40;
	textLabel->fontScale = 0.6f;
	textLabel->letterSpacingPx = 1.0f;
	textLabel->centered = true;

	// Arrow holder

	Entity* arrowHolderEntity = entityStorage->CreateEntity("ArrowHolder", itemEntity);

	transform2D = arrowHolderEntity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->position = point3d(-0.98f, 0, 0);
	transform2D->scale = point3d(0.9f, 0.9f, 0);
	transform2D->ratio = ScreenAspectRatio::YY;

	// Arrow

	Entity* arrowEntity = entityStorage->CreateEntity("Arrow", arrowHolderEntity);

	transform2D = arrowEntity->AddComponent<Transform2D>();

	ImageLabel* imageLabel = arrowEntity->AddComponent<ImageLabel>();
	imageLabel->textureName = "itemArrow";

	item.button = itemEntity;
}


ExplorerItem EditorUI::NewItem(Entity* entity)
{
	ExplorerItem item = ExplorerItem(entity);

	for (Entity* child : entity->GetChildren()) {
		item.children.push_back(NewItem(child));
	}
	SortItemsAlphabetically(item.children);

	return item;
}


void EditorUI::SortItemsAlphabetically(vector<ExplorerItem>& list)
{
	int size = list.size();
	for (int i = 1; i < size; i++) {
		ExplorerItem item = list[i];
		string& itemName = item.entity->name;

		int j = i - 1;
		while (j >= 0 && isFirstStringHigher(itemName, list[j].entity->name)) {
			list[j + 1] = list[j];
			j--;
		}

		list[j + 1] = item;
	}
}