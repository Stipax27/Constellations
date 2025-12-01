////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerController.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYERCONTROLLER_H_
#define _PLAYERCONTROLLER_H_


//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"
#include "mouseclass.h"
#include "windowclass.h"

#include "entity.h"

#include "Transform.cpp"
#include "PhysicBody.cpp"
#include "Constellation.cpp"


/////////////
// GLOBALS //
/////////////
const float MOUSE_SENSITIVITY = 0.002f;
const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.45f;
const float SENSIVITY = 2.0f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;
const float ROLL_SPEED = 1.0f;


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerController
////////////////////////////////////////////////////////////////////////////////

class PlayerController
{
public:
	PlayerController();
	PlayerController(const PlayerController&);
	~PlayerController();

	void Initialize(Entity*, CameraClass*, MouseClass*, WindowClass*);
	void Shutdown();

	void ProcessInput();
	void ProcessCamera();
	void ProcessMouse();

private:
	Entity* playerEntity;

	Transform* playerTransform;
	PhysicBody* playerPhysicBody;
	Constellation* playerConstellation;

	CameraClass* camera;
	MouseClass* mouse;
	WindowClass* window;
	bool ShiftFlag;

	bool mousePressed = false;
};

#endif