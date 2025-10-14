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


void PlayerController::ProcessMouse()
{
	float dPitch = 0.0f, dYaw = 0.0f, dRoll = 0.0f;

	float x = mouse->pos.x - window->width / 2;
	float y = mouse->pos.y - window->height / 2;

	point3d mousePos = point3d(x / window->width / window->aspect, y / window->height, 0);
	float length = mousePos.magnitude();

	if (length > CURSOR_IGNORE_ZONE)
	{
		if (length > MAX_CURSOR_DEVIATION)
		{
			mousePos = mousePos.normalized() * MAX_CURSOR_DEVIATION;
			SetCursorPos(mousePos.x * window->width * window->aspect + window->width / 2, mousePos.y * window->height + window->height / 2);
		}

		float k = (length - CURSOR_IGNORE_ZONE) / MAX_CURSOR_DEVIATION;

		float dx = (mousePos.x) * SENSIVITY * k;
		float dy = (mousePos.y) * SENSIVITY * k;

		dPitch = dy;
		dYaw = dx;
	}

	// сопюбкемхе Q/E дкъ бпюыемхъ
	if (IsKeyPressed('E')) {
		dRoll -= ROLL_SPEED;
		//Camera::state.n = lerp(Camera::state.n, 100, 0.3f);
	}
	if (IsKeyPressed('Q')) {
		dRoll += ROLL_SPEED;
		//Camera::state.n = lerp(Camera::state.n, 100, 0.3f);
	}

	// опхлемъел бпюыемхе й цепнч (сопюбкъеряъ лшьэч)
	if (dPitch != 0.0f || dYaw != 0.0f || dRoll != 0.0f) {
		XMVECTOR qPitch = XMQuaternionRotationAxis(Hero::state.Right, dPitch);
		XMVECTOR qYaw = XMQuaternionRotationAxis(Hero::state.Up, dYaw);
		XMVECTOR qRoll = XMQuaternionRotationAxis(Hero::state.Forwardbuf, dRoll);

		XMVECTOR qTotal = XMQuaternionMultiply(qYaw, qPitch);
		qTotal = XMQuaternionMultiply(qTotal, qRoll);

		Hero::state.currentRotation = XMQuaternionMultiply(Hero::state.currentRotation, qTotal);
		Hero::state.currentRotation = XMQuaternionNormalize(Hero::state.currentRotation);

		// намнбкъел бейрнпш цепнъ
		Hero::state.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
		Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
		Hero::state.Right = XMVector3Cross(Hero::state.Up, Hero::state.Forwardbuf);

		// йюлепю асдер окюбмн якеднбюрэ гю щрхл онбнпнрнл
	}
	else {
		Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
	}
}