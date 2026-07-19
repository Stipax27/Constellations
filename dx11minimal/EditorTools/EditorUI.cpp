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

}


void EditorUI::InitExplorer()
{
	UpdateEntityList();

	Entity* explorerWindow = entityStorage->CreateEntity("Explorer");

	Transform2D* transform2D = explorerWindow->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->position = point3d(-1, 0, 0);
	transform2D->scale = point3d(EXPLORER_WIDTH, 1, 0);

	Rect* rect = explorerWindow->AddComponent<Rect>();
	rect->color = point3d(0.25f, 0.25f, 0.25f);
	rect->opacity = 0.75f;
}


void EditorUI::UpdateEntityList()
{
	vector<ExplorerItem> list;

	for (Entity* entity : entityStorage->entities) {
		if (entity->GetParent() == nullptr) {
			ExplorerItem item = CreateExplorerItem(entity);
			list.push_back(item);
		}
	}

	for (ExplorerItem& item : list) {

	}
}


ExplorerItem EditorUI::CreateExplorerItem(Entity* entity)
{
	ExplorerItem item = ExplorerItem(entity);

	for (Entity* child : entity->GetChildren()) {
		item.children.push_back(CreateExplorerItem(child));
	}
	SortItemsAlphabetically(item.children);

	return item;
}


void EditorUI::SortItemsAlphabetically(vector<ExplorerItem>& list)
{
	int size = list.size();
	for (int i = 1; i < size; i++) {
		ExplorerItem& item = list[i];
		string& itemName = item.entity->name;

		int j = i - 1;
		while (j >= 0 && isFirstStringHigher(list[j].entity->name, itemName)) {
			list[j + 1] = list[j];
			j--;
		}

		list[j + 1] = item;
	}
}