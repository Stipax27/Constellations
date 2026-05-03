////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerController.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYERCONTROLLER_H_
#define _PLAYERCONTROLLER_H_


//////////////
// INCLUDES //
//////////////
#include "../Lib/isingleton.h"
#include "../Lib/input.h"

#include "../Camera/cameraclass.h"
#include "../Mouse/mouseclass.h"
#include "../Window/windowclass.h"

#include "../Lib/timer.h"
#include "../ECS_Base/world.h"
#include "../ECS_Base/entity.h"

#include "../BasicComponents/Transform.h"
#include "../Physic/Movement/PhysicBody.h"
#include "../Render/PointCloud.h"
#include "../Render/Sprite/Sprite.h"
#include "../Compute/Combat/Health.h"
#include "CameraTarget.h"

#include "PlayerAbilities.h"
#include "Combo/comboManager.h"


/////////////
// GLOBALS //
/////////////
#define SENSIVITY 3.5f

const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.45f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;

#define ROLL_SPEED 1.0f
#define PLAYER_MOVE_SPEED 1000.0f

#define DASH_SPEED 200.0f
#define DASH_AIR_FRICTION 0.35f
#define DASH_COST 50.0f
#define DASH_CD 1000

#define INTERACT_DISTANCE 5


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerController
////////////////////////////////////////////////////////////////////////////////

class PlayerController
{
public:
	PlayerAbilities* abilities;

public:
	PlayerController();
	PlayerController(const PlayerController&);
	~PlayerController();

	void Initialize(Entity*);
	void Shutdown();

	void ProcessInput();
	void ProcessCamera();
	void ProcessMouse();
	void ProccessUI();

private:
	ComboManager* comboManager;

	EntityStorage* entityStorage;
	CollisionManagerClass* collisionManager;

	Entity* playerEntity;
	Entity* ui;

	Transform* playerTransform;
	PhysicBody* playerPhysicBody;
	PointCloud* playerPointCloud;

	Entity* elementLabel;

	Entity* target;

	CameraClass* camera;
	MouseClass* mouse;
	WindowClass* window;

	bool movementLocked = false;

	DWORD lastDashTime = 0;

	Entity* cameraTarget;
	bool lockMovementOnTarget;

private:
	void ClickOnObjectL();
	void ClickOnObjectR();
	void GameReset();
};

#endif