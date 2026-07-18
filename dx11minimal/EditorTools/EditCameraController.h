#ifndef _EDITOR_CAMERA_CONTROLLER_H_
#define _EDITOR_CAMERA_CONTROLLER_H_

#include "../Engine/Camera/cameraclass.h"
#include "../Engine/Mouse/mouseclass.h"
#include "../Engine/Window/windowclass.h"


#define CAMERA_FLY_SPEED 15.0f
#define CAMERA_FAST_FLY_SPEED 60.0f
#define CAMERA_SLOW_FLY_SPEED 3.0f


class EditCameraController : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	CameraClass* camera;
	MouseClass* mouse;
	WindowClass* window;

	point3d mousePos;
	point3d mouseAbsPos;

private:
	void ProcessPosition();
	void ProcessRotation();
};

#endif
