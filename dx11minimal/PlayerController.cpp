#include "PlayerController.h"


PlayerController::PlayerController()
{
	playerTransform = 0;
	playerPhysicBody = 0;
	playerConstellation = 0;

	camera = 0;
	mouse = 0;
	window = 0;
}

PlayerController::PlayerController(const PlayerController& other)
{
}


PlayerController::~PlayerController()
{
}


void PlayerController::Initialize(Entity* Player, CameraClass* Camera, MouseClass* Mouse, WindowClass* Window)
{
	playerTransform = Player->GetComponent<Transform>();
	playerPhysicBody = Player->GetComponent<PhysicBody>();
	playerConstellation = Player->GetComponent<Constellation>();

	camera = Camera;
	mouse = Mouse;
	window = Window;
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

	if (camera)
	{
		camera = 0;
	}

	if (mouse)
	{
		mouse = 0;
	}

	if (window)
	{
		window = 0;
	}
}


bool PlayerController::IsKeyPressed(const int Key)
{
	return GetAsyncKeyState(Key) & 0x8000;
}


void PlayerController::ProcessInput()
{
	// УПРАВЛЕНИЕ  ПЕРЕДВИЖЕНИЕМ
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

	// УПРАВЛЕНИЕ НАКЛОНОМ
	float roll = 0.0f;

	if (IsKeyPressed('E')) {
		roll = -ROLL_SPEED;
	}
	if (IsKeyPressed('Q')) {
		roll = ROLL_SPEED;
	}

	if (roll != 0) {
		XMVECTOR lookVector = XMQuaternionRotationAxis(playerTransform->GetLookVector(), roll);
		//camera->AddQuaternionRotation(XMVectorGetX(lookVector), XMVectorGetY(lookVector), XMVectorGetZ(lookVector), XMVectorGetW(lookVector));
	}
}


void PlayerController::ProcessCamera()
{
	camera->SetPosition(playerTransform->position.x, playerTransform->position.y, playerTransform->position.z - 10);
}


void PlayerController::ProcessMouse()
{
	float dPitch = 0.0f, dYaw = 0.0f;

	float x = mouse->pos.x - window->width / 2;
	float y = mouse->pos.y - window->height / 2;

	point3d mousePos = point3d(x / window->width / window->aspect, y / window->height, 0);
	float length = mousePos.magnitude();

	if (length > CURSOR_IGNORE_ZONE) {
		if (length > MAX_CURSOR_DEVIATION) {
			mousePos = mousePos.normalized() * MAX_CURSOR_DEVIATION;
			SetCursorPos(mousePos.x * window->width * window->aspect + window->width / 2, mousePos.y * window->height + window->height / 2);
		}

		float k = (length - CURSOR_IGNORE_ZONE) / MAX_CURSOR_DEVIATION;

		dYaw = mousePos.x;
		dPitch = mousePos.y;

		XMVECTOR addRotation = eulerToQuanternion(dPitch, dYaw, 0) * SENSIVITY * k;
		//playerPhysicBody->qAngVelocity = XMQuaternionMultiply(playerTransform->qRotation, addRotation);

		camera->AddMatrixRotation(XMMatrixRotationY(XMConvertToRadians(dYaw * SENSIVITY * k)));
		camera->AddMatrixRotation(XMMatrixRotationX(XMConvertToRadians(dPitch * SENSIVITY * k)));
	}
	else {
		//Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
	}
}