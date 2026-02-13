#include "PlayerController.h"


PlayerController::PlayerController()
{
	playerEntity = 0;

	playerTransform = 0;
	playerPhysicBody = 0;
	playerPointCloud = 0;

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


void PlayerController::Initialize(Entity* Player, World* m_World, MouseClass* Mouse, WindowClass* Window, CollisionManagerClass* CollisionManager)
{
	playerEntity = Player;

	playerTransform = Player->GetComponent<Transform>();
	playerPhysicBody = Player->GetComponent<PhysicBody>();
	playerPointCloud = Player->GetComponent<PointCloud>();
	playerHealth = Player->GetComponent<Health>();

	ui = m_World->entityStorage->GetEntityByName("UI");
	healthBar = ui->GetChildByName("HealthBar", true);
	staminaBar = ui->GetChildByName("StaminaBar", true);

	camera = m_World->m_Camera;
	mouse = Mouse;
	window = Window;

	abilities = new PlayerAbilities;
	abilities->Initialize(m_World, camera, playerEntity, CollisionManager);
}


void PlayerController::Shutdown()
{
	if (abilities) {
		abilities->Shutdown();
		delete abilities;
		abilities = 0;
	}

	if (playerEntity)
		playerEntity = 0;

	if (playerTransform)
		playerTransform = 0;

	if (playerPhysicBody)
		playerPhysicBody = 0;

	if (playerPointCloud)
		playerPointCloud = 0;

	if (playerHealth)
		playerHealth = 0;

	if (ui)
		ui = 0;

	if (healthBar)
		healthBar = 0;

	if (staminaBar)
		staminaBar = 0;

	if (camera)
		camera = 0;

	if (mouse)
		mouse = 0;

	if (window)
		window = 0;
}


void PlayerController::ProcessInput()
{
	if (playerEntity != nullptr && playerEntity->IsActive()) {

		if (movementLocked && playerPhysicBody->velocity.magnitude() < PLAYER_MOVE_SPEED) {
			movementLocked = false;
			playerPhysicBody->airFriction = 1;
		}

		if (!movementLocked)
		{
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
				point3d newVelocity = playerPhysicBody->velocity + velocity.normalized();
				if (newVelocity.magnitude() > PLAYER_MOVE_SPEED) {
					playerPhysicBody->velocity = newVelocity.normalized() * PLAYER_MOVE_SPEED;
				}
				else {
					playerPhysicBody->velocity = newVelocity;
				}
			}
		}

		if (IsKeyPressed(VK_LSHIFT)) {
			Dash();
		}
		
		float roll = 0.0f;

		if (IsKeyPressed('E')) {
			roll = -ROLL_SPEED;
		}
		if (IsKeyPressed('Q')) {
			roll = ROLL_SPEED;
		}

		if (roll != 0) {
			playerPhysicBody->mAngVelocity = XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), roll * RAD);
		}

		if (IsKeyPressed('1')) {
			abilities->weapon = PlayerWeapons::Fists;
		}
		if (IsKeyPressed('2')) {
			abilities->weapon = PlayerWeapons::Sword;
		}
		if (IsKeyPressed('3')) {
			abilities->weapon = PlayerWeapons::Bow;
		}
	}
}


void PlayerController::ProcessCamera()
{
	camera->SetPosition(camera->GetPosition().lerp(playerTransform->position - playerTransform->GetLookVector() * camera->distance + playerTransform->GetUpVector() * 2, 0.2f));
	camera->SetMatrixRotation(playerTransform->mRotation);
}


void PlayerController::ProcessMouse()
{
	float x = mouse->absolutePos.x - window->width / 2;
	float y = mouse->absolutePos.y - window->height / 2;
	point3d mousePos = point3d(x / window->width / window->aspect, y / window->height, 0);

	switch (mouse->state)
	{
	case MouseState::Centered:
	{
		if (playerEntity != nullptr && playerEntity->IsActive()) {
			float length = mousePos.magnitude();

			if (length > CURSOR_IGNORE_ZONE) {
				if (length > MAX_CURSOR_DEVIATION) {
					mousePos = mousePos.normalized() * MAX_CURSOR_DEVIATION;

					mouse->absolutePos = point3d(mousePos.x * window->width * window->aspect + window->width / 2, mousePos.y * window->height + window->height / 2, 0);
					mouse->pos = point3d(mouse->absolutePos.x / window->width * 2 - 1, -(mouse->absolutePos.y / window->height * 2 - 1), 0.0f);

					SetCursorPos(mouse->absolutePos.x, mouse->absolutePos.y);
				}

				float k = (length - CURSOR_IGNORE_ZONE) / MAX_CURSOR_DEVIATION;
				mousePos *= SENSIVITY * k;

				//XMVECTOR addRotation = eulerToQuanternion(dPitch, dYaw, 0) * SENSIVITY * k;
				XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mousePos.y), XMConvertToRadians(mousePos.x), 0);

				playerPhysicBody->mAngVelocity = playerPhysicBody->mAngVelocity * additionalRotation;
			}
			//else {
				//Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
			//}

			if (mouse->IsLButtonDown()) {
				abilities->Charging();
			}
			else if (mouse->IsLButtonUnclicked()) {
				abilities->Attack(*playerTransform, mouse->GetMouseRay());
			}

			if (mouse->IsRButtonClicked()) {
				abilities->BlockStart();
			}
			else if (mouse->IsRButtonUnclicked()) {
				abilities->BlockEnd();
			}
		}
		break;
	}
	case MouseState::Free:
	{
		if (mouse->IsLButtonClicked() && mouse->visible)
		{
			//ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Mouse_click1.wav");
			//point3d mousePos = point3d(mouse.pos.x / width * 2 - 1, -(mouse.pos.y / height * 2 - 1), 0);

			for (int i = 0; i < 20; i++)
			{
				MouseParticle particle = MouseParticle();
				particle.pos = mouse->pos;
				particle.angle = (float)getRandom(0, 100) / 100.0f * PI * 2.0f;
				//particle.vel = point3d(getRandom(-100, 100), getRandom(-100, 100), 0).normalized() * point3d(window->aspect, 1, 0) * (float)getRandom(8, 30) / 100.0f * 0.002f;
				particle.lifetime = getRandom(500, 1500);
				particle.startTime = timer::currentTime;

				mouse->particles.push_back(particle);
			}
		}
		break;
	}
	}

}


void PlayerController::ProccessUI()
{
	Transform2D* healthTransform = healthBar->GetComponent<Transform2D>();
	healthTransform->scale = point3d(playerHealth->hp / playerHealth->maxHp, 1, 0);

	Transform2D* staminaTransform = staminaBar->GetComponent<Transform2D>();
	staminaTransform->scale = point3d(abilities->stamina / abilities->maxStamina, 1, 0);
}


void PlayerController::Dash()
{
	if (!movementLocked && timer::currentTime >= lastDashTime + DASH_CD && abilities->stamina >= DASH_COST) {
		lastDashTime = timer::currentTime;

		point3d velocity;
		if (playerPhysicBody->velocity.magnitude() > 0) {
			velocity = playerPhysicBody->velocity.normalized();
		}
		else {
			velocity = playerTransform->GetLookVector();
		}

		playerPhysicBody->velocity = velocity * DASH_SPEED;
		movementLocked = true;
		playerPhysicBody->airFriction = DASH_AIR_FRICTION;

		abilities->stamina -= DASH_COST;
	}
}