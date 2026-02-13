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
#include "PointCloud.h"
#include "Health.h"

#include "PlayerAbilities.h"


/////////////
// GLOBALS //
/////////////
#define SENSIVITY 1.5f

const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.45f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;

#define ROLL_SPEED 1.0f
#define PLAYER_MOVE_SPEED 15.0f

#define DASH_SPEED 75.0f
#define DASH_AIR_FRICTION 5.0f
#define DASH_COST 50.0f
#define DASH_CD 1000


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

	void Initialize(Entity*, World*, MouseClass*, WindowClass*, CollisionManagerClass*);
	void Shutdown();

	void ProcessInput();
	void ProcessCamera();
	void ProcessMouse();
	void ProccessUI();

	// Ќовый метод дл€ обработки получени€ урона
	void TakeDamage(float damage);

private:
	Entity* playerEntity;
	Entity* ui;

	Transform* playerTransform;
	PhysicBody* playerPhysicBody;
	PointCloud* playerPointCloud;
	Health* playerHealth;

	Entity* healthBar;
	Entity* staminaBar;

	CameraClass* camera;
	MouseClass* mouse;
	WindowClass* window;

	bool movementLocked = false;

	DWORD lastDashTime = 0;

private:
	void Dash();
};

#endif