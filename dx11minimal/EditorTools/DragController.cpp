#include "DragController.h"

#include "../Engine/Lib/input.h"
#include "../Engine/Lib/timer.h"
#include "../Engine/Lib/logging.h"

#include "../Engine/Utils/componentutils.h"

#include "../GlobalConfigs.h"

#include <format>
#include <string>

using namespace std;


void DragController::Initialize()
{
	camera = Singleton::GetInstance<CameraClass>();
	mouse = Singleton::GetInstance<MouseClass>();
	collisionManager = Singleton::GetInstance<CollisionManagerClass>();
	entityStorage = Singleton::GetInstance<EntityStorage>();
}


void DragController::Shutdown()
{
	if (camera)
		camera = 0;

	if (mouse)
		mouse = 0;

	if (collisionManager)
		collisionManager = 0;

	if (entityStorage)
		entityStorage = 0;
}


void DragController::Update()
{
	ProcessPivotDrag();
	ProcessSave();
}


void DragController::ProcessPivotDrag()
{
	if (mouse->IsLButtonClicked()) {
		point3d mouseDirection = mouse->GetMouseDirection();
		RayInfo rayInfo = RayInfo(camera->position, mouseDirection * SELECT_DISTANCE, CollisionFilter::Group::_editorSelect, false);
		RaycastResult result = collisionManager->Raycast(rayInfo);

		if (!result.hit || result.entity == nullptr)
			return;

		pair<Entity*, Transform*> ancestor = result.entity->GetUpperAncestorWithComponent<Transform>();
		if (!ancestor.first || !ancestor.second)
			return;

		dragEntity = ancestor.first;
		dragTransform = ancestor.second;

		dragDistance = (result.position - camera->position).magnitude();
		dragOffset = dragTransform->position - result.position;
	}
	else if (mouse->IsLButtonReleased()) {
		dragEntity = nullptr;
		dragTransform = nullptr;
	}

	DragByPivot();
}


void DragController::DragByPivot()
{
	if (!dragEntity || !dragTransform)
		return;

	point3d mousePos = camera->position + mouse->GetMouseDirection() * dragDistance + dragOffset;
	dragTransform->position = mousePos;
}


void DragController::ProcessSave()
{
	if (input::IsKeyDown(VK_LCONTROL) && input::IsKeyDown(VK_LSHIFT) && input::IsKeyPressed('S')) {
		for (Entity* entity : entityStorage->entities) {
			if (entity->GetParent() == nullptr) {
				entityStorage->SaveEntityToFile(entity, entity->name);
			}
		}
	}
}