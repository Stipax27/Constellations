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
	Entity* explorerWindow = entityStorage->CreateEntity("Explorer");

	Transform2D* transform2D = explorerWindow->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->position = point3d(-1, 0, 0);
	transform2D->scale = point3d(EXPLORER_WIDTH, 1, 0);

	Rect* rect = explorerWindow->AddComponent<Rect>();
	rect->color = point3d(0.25f, 0.25f, 0.25f);
	rect->opacity = 0.75f;
}