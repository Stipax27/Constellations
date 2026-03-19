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


void PlayerController::Initialize(Entity* Player)
{
	playerEntity = Player;

	playerTransform = Player->GetComponent<Transform>();
	playerPhysicBody = Player->GetComponent<PhysicBody>();
	playerPointCloud = Player->GetComponent<PointCloud>();
	playerHealth = Player->GetComponent<Health>();

	World* world = Singleton::GetInstance<World>();

	ui = world->entityStorage->GetEntityByName("UI");
	healthBar = ui->GetChildByName("HealthBar", true);
	staminaBar = ui->GetChildByName("StaminaBar", true);

	camera = world->m_Camera;
	mouse = Singleton::GetInstance<MouseClass>();
	window = Singleton::GetInstance<WindowClass>();

	currentMaxSpeed = PLAYER_MOVE_SPEED;
	isRunning = false;

	abilities = new PlayerAbilities;
	abilities->Initialize(playerEntity);
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
	if (playerEntity == nullptr || !playerEntity->IsActive())
		return;

	isRunning = false;

	// Проверяем Shift и наличие выносливости
	if (IsKeyPressed(VK_SHIFT) && abilities->stamina > 0) {
		isRunning = true;
		currentMaxSpeed = PLAYER_MOVE_SPEED * 5.0f;

		// Тратим выносливость только если двигаемся
		if (IsKeyPressed('W') || IsKeyPressed('S') || IsKeyPressed('A') || IsKeyPressed('D')) {
			abilities->stamina = max(0.0f, abilities->stamina - 0.5f); 
		}
	}
	else {
	// Если Shift не нажат - возвращаем обычную скорость
	currentMaxSpeed = PLAYER_MOVE_SPEED;
	}

	// Восстанавливаем выносливость, если не бежим
	if (!isRunning && abilities->stamina < abilities->maxStamina) {
		abilities->stamina = min(abilities->maxStamina, abilities->stamina + 0.2f);
	}


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
		if (IsKeyPressed(VK_LMENU)) {
			velocity += playerTransform->GetUpVector();
		}
		if (IsKeyPressed(VK_CONTROL)) {
			velocity += playerTransform->GetUpVector() * -1;
		}

		if (velocity.magnitude() > 0) {
			point3d newVelocity = playerPhysicBody->velocity + velocity.normalized();
			if (newVelocity.magnitude() > currentMaxSpeed) {
				playerPhysicBody->velocity = newVelocity.normalized() * currentMaxSpeed;
			}
			else {
				playerPhysicBody->velocity = newVelocity;
			}
		}
	}

		// Обработка кнопки F для щита
	static bool fKeyPressed = false;
	if (IsKeyPressed('F') && !fKeyPressed) {
		fKeyPressed = true;
		abilities->ShieldStart(); // Активируем щит при нажатии F
	}
	else if (!IsKeyPressed('F') && fKeyPressed) {
		fKeyPressed = false;
		abilities->ShieldEnd(); // Деактивируем щит при отпускании F
	}

	
		float roll = 0.0f;
	if (IsKeyPressed(VK_SPACE)) {
		Dash();
	}
		
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

	if (IsKeyPressed('T')) {
		abilities->TimestopStart();
	}
	if (IsKeyPressed('Y')) {
		abilities->TimestopEnd();
	}

	static bool vKeyPressed = false;
	if (IsKeyPressed('V') && !vKeyPressed) {
		vKeyPressed = true;
		if (abilities) {
			abilities->ParticleVacuumStart();
		}
	}
	else if (!IsKeyPressed('V') && vKeyPressed) {
		vKeyPressed = false;
		if (abilities) {
			abilities->ParticleVacuumEnd();  
		}
	}

	static bool gKeyPressed = false;
	if (IsKeyPressed('G') && !gKeyPressed) {
		gKeyPressed = true;
		if (abilities) {
			abilities->BlowGasStart(); 
		}
	}
	else if (!IsKeyPressed('G') && gKeyPressed) {
		gKeyPressed = false;
		if (abilities) {
			abilities->BlowGasEnd();
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

				XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mousePos.y), XMConvertToRadians(mousePos.x), 0);

				playerPhysicBody->mAngVelocity = playerPhysicBody->mAngVelocity * additionalRotation;
			}

			// Обработка атак мышью - проверяем что щит не активен
			if (!abilities->IsShieldActive()) {
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
		}
		break;
	}
	case MouseState::Free:
	{
		if (mouse->IsLButtonClicked() && mouse->visible)
		{
			for (int i = 0; i < 20; i++)
			{
				MouseParticle particle = MouseParticle();
				particle.pos = mouse->pos;
				particle.angle = (float)getRandom(0, 100) / 100.0f * PI * 2.0f;
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

// Добавьте этот метод в PlayerController для обработки получения урона
void PlayerController::TakeDamage(float damage)
{
	if (!playerEntity->IsActive()) return;

	// Пытаемся заблокировать урон щитом
	if (!abilities->TryBlockDamage(damage)) {
		// Если не заблокировали - применяем урон
		playerHealth->hp -= damage;

		// Визуальный эффект получения урона
		playerPointCloud->color = point3d(1.0f, 0.0f, 0.0f);
		// Возвращаем цвет через некоторое время
	}
}