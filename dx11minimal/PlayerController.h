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

#include "timer.h"
#include "world.h"
#include "entity.h"

#include "Transform.h"
#include "PhysicBody.h"
#include "Constellation.h"

#include "PlayerAbilities.h"


/////////////
// GLOBALS //
/////////////
const float SENSIVITY = 1.5f;
const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.45f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;
const float ROLL_SPEED = 1.0f;
const float PLAYER_MOVE_SPEED = 15.0f;
const float DASH_SPEED = 75.0f;
const float DASH_AIR_FRICTION = 5.0f;

const DWORD DASH_CD = 1000;


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerController
////////////////////////////////////////////////////////////////////////////////

class PlayerController
{
public:
	PlayerController();
	PlayerController(const PlayerController&);
	~PlayerController();

	void Initialize(Entity*, World*, MouseClass*, WindowClass*);
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

	PlayerAbilities* abilities;

	bool movementLocked = false;
	bool mousePressed = false;

	DWORD lastDashTime = 0;
};

#endif