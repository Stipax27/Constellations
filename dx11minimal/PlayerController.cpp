#include "PlayerController.h"


PlayerController::PlayerController()
{
	playerTransform = 0;
	playerPhysicBody = 0;
	playerConstellation = 0;
}

PlayerController::PlayerController(const PlayerController& other)
{
}


PlayerController::~PlayerController()
{
}


void PlayerController::Initialize(Entity* Player, CameraClass* Camera)
{
	playerTransform = Player->GetComponent<Transform>();
	playerPhysicBody = Player->GetComponent<PhysicBody>();
	playerConstellation = Player->GetComponent<Constellation>();

	camera = Camera;
}


void PlayerController::Shutdown()
{
	if (playerTransform)
	{
		playerTransform = 0;
	}

	if (playerPhysicBody)
	{
		playerPhysicBody = 0;
	}

	if (playerConstellation)
	{
		playerConstellation = 0;
	}
}


bool PlayerController::IsKeyPressed(const int Key)
{
	return GetAsyncKeyState(Key) & 0x8000;
}


void PlayerController::ProcessInput()
{
	playerPhysicBody->velocity = point3d();

	if (IsKeyPressed('W')) {
		playerPhysicBody->velocity += point3d(0, 0, 1);
	}
	if (IsKeyPressed('S')) {
		playerPhysicBody->velocity += point3d(0, 0, -1);
	}
	if (IsKeyPressed('A')) {
		playerPhysicBody->velocity += point3d(-1, 0, 0);
	}
	if (IsKeyPressed('D')) {
		playerPhysicBody->velocity += point3d(1, 0, 0);
	}
	if (IsKeyPressed(VK_SPACE)) {
		playerPhysicBody->velocity += point3d(0, 1, 0);
	}
	if (IsKeyPressed(VK_CONTROL)) {
		playerPhysicBody->velocity += point3d(0, -1, 0);
	}

	if (playerPhysicBody->velocity.magnitude() > 0) {
		playerPhysicBody->velocity = playerPhysicBody->velocity.normalized() * 5;
	}
}


void PlayerController::ProcessCamera()
{
	camera->SetPosition(playerTransform->position.x, playerTransform->position.y, playerTransform->position.z - 10);
}