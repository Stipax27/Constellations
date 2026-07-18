#include "editorclass.h"

using namespace std;


void EditorClass::Initialize()
{
	editCameraController = Singleton::GetInstance<EditCameraController>();
	dragController = Singleton::GetInstance<DragController>();
}


void EditorClass::Shutdown()
{
	if (editCameraController) {
		editCameraController->Shutdown();
		delete editCameraController;
		editCameraController = 0;
	}

	if (dragController) {
		dragController->Shutdown();
		delete dragController;
		dragController = 0;
	}
}


void EditorClass::Update()
{
	editCameraController->Update();
	dragController->Update();
}