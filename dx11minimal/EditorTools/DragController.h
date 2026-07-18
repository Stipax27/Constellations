#ifndef _DRAG_CONTROLLER_H_
#define _DRAG_CONTROLLER_H_

#include "../Engine/Camera/cameraclass.h"
#include "../Engine/Mouse/mouseclass.h"
#include "../Engine/Window/windowclass.h"



class DragController : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	CameraClass* camera;
	MouseClass* mouse;

private:
	void ProcessPivotDrag();
};

#endif
