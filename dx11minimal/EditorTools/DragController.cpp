#include "DragController.h"

#include "../Engine/Lib/input.h"
#include "../Engine/Lib/timer.h"
#include "../Engine/Lib/logging.h"

#include "../GlobalConfigs.h"

#include <format>
#include <string>


void DragController::Initialize()
{
	camera = Singleton::GetInstance<CameraClass>();
	mouse = Singleton::GetInstance<MouseClass>();
}


void DragController::Shutdown()
{
	if (camera)
		camera = 0;

	if (mouse)
		mouse = 0;
}


void DragController::Update()
{
	ProcessPivotDrag();
}


void DragController::ProcessPivotDrag()
{
	if (mouse->IsLButtonDown()) {
		
	}
}