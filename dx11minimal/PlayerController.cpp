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
	point3d velocity = point3d();

	if (IsKeyPressed('W')) {
		velocity += playerTransform->GetLookVector();
	}
	if (IsKeyPressed('S')) {
		velocity += playerTransform->GetLookVector() * -1;
	}
	if (IsKeyPressed('A')) {
		velocity += playerTransform->GetRightVector() * -1;
	}
	if (IsKeyPressed('D')) {
		velocity += playerTransform->GetRightVector();
	}
	if (IsKeyPressed(VK_SPACE)) {
		velocity += playerTransform->GetUpVector();
	}
	if (IsKeyPressed(VK_CONTROL)) {
		velocity += playerTransform->GetUpVector() * -1;
	}

	if (velocity.magnitude() > 0) {
		playerPhysicBody->velocity = velocity.normalized() * 15;
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
		//camera->AddVectorRotation(playerTransform->GetLookVector(), roll);

		playerPhysicBody->mAngVelocity = XMMatrixRotationAxis(playerTransform->GetLookVector().toXMVector(), roll * RAD);
	}
}


void PlayerController::ProcessCamera()
{
	camera->SetPosition(playerTransform->position - playerTransform->GetLookVector() * 10);
	camera->SetMatrixRotation(playerTransform->mRotation);
}


void PlayerController::ProcessMouse()
{
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
		mousePos *= SENSIVITY * k;

		//XMVECTOR addRotation = eulerToQuanternion(dPitch, dYaw, 0) * SENSIVITY * k;
		XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mousePos.y), XMConvertToRadians(mousePos.x), 0);

		playerPhysicBody->mAngVelocity = playerPhysicBody->mAngVelocity * additionalRotation;

		//camera->AddMatrixRotation(additionalRotation);
	}
	else {
		//Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
	}
}