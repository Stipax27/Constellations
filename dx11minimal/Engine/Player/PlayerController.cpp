#include "PlayerController.h"

#include "../UI/Text/TextLabel.h"

using namespace std;


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


void PlayerController::Initialize(Entity* Player, EntityStorage* entityStorage)
{
	playerEntity = Player;

	playerTransform = Player->GetComponent<Transform>();
	playerPhysicBody = Player->GetComponent<PhysicBody>();
	playerPointCloud = Player->GetComponent<PointCloud>();

	World* world = Singleton::GetInstance<World>();

	ui = world->entityStorage->GetEntityByName("UI");
	elementLabel = ui->GetChildByName("ElementLabel", true);

	camera = world->m_Camera;
	mouse = Singleton::GetInstance<MouseClass>();
	window = Singleton::GetInstance<WindowClass>();
	entityStorage = Singleton::GetInstance<EntityStorage>();
	collisionManager = Singleton::GetInstance<CollisionManagerClass>();

	abilities = Singleton::GetInstance<PlayerAbilities>();
	abilities->Initialize(playerEntity, entityStorage);

	comboManager = Singleton::GetInstance<ComboManager>();
	comboManager->Initialize();

	cameraTarget = nullptr;
	lockMovementOnTarget = false;

	SetCursorPos(window->width / 2, window->height / 2);
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

	if (ui)
		ui = 0;

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

	if (movementLocked && playerPhysicBody->velocity.magnitude() < PLAYER_MOVE_SPEED) {
		movementLocked = false;
		playerPhysicBody->airFriction = 1;
	}

	if (!movementLocked)
	{
		point3d velocity = point3d();

		if (input::IsKeyDown('W')) {
			velocity += playerTransform->GetLookVector();
		}
		if (input::IsKeyDown('S')) {
			velocity += playerTransform->GetLookVector() * -1;
		}
		if (input::IsKeyDown('A')) {
			velocity += playerTransform->GetRightVector() * -1;
		}
		if (input::IsKeyDown('D')) {
			velocity += playerTransform->GetRightVector();
		}

		if (velocity.magnitude() > 0) {
			playerPhysicBody->acceleration += velocity.normalized() * PLAYER_MOVE_SPEED;
		}
	}

	if (input::IsKeyPressed('1') && abilities->weapon != PlayerWeapons::Fists) {
		abilities->weapon = PlayerWeapons::Fists;
		comboManager->SaveInput(ComboInputType::TakeFists);
	}
	if (input::IsKeyPressed('2') && abilities->weapon != PlayerWeapons::Sword) {
		abilities->weapon = PlayerWeapons::Sword;
		comboManager->SaveInput(ComboInputType::TakeSword);
	}
	if (input::IsKeyPressed('3') && abilities->weapon != PlayerWeapons::Bow) {
		abilities->weapon = PlayerWeapons::Bow;
		comboManager->SaveInput(ComboInputType::TakeBow);
	}

	if (input::IsKeyPressed('T')) {
		abilities->Timestop();
	}

	/*if (input::IsKeyPressed('G')) {
		abilities->Grab();
	}*/

	if (input::IsKeyPressed('P')) {
		int i = (int)abilities->element;
		i = i < 4 ? i + 1 : 0;
		abilities->element = (Elements)i;
	}

	if (input::IsKeyPressed('O')) {
		int i = (int)abilities->element;
		i = i > 0 ? i - 1 : 4;
		abilities->element = (Elements)i;
	}

}
		


void PlayerController::ProcessCamera()
{
	camera->position = camera->position.lerp(playerTransform->position + playerTransform->GetUpVector() * camera->distance, 0.4f);

	XMMATRIX matrixRotation = GetMatrixFromDirection(point3d(0, -1, 0), playerTransform->GetLookVector());

	camera->SetMatrixRotation(LerpMatrix(camera->GetMatrixRotation(), matrixRotation, 0.15f));
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

				if (!lockMovementOnTarget || cameraTarget == nullptr) {
					float k = (length - CURSOR_IGNORE_ZONE) / MAX_CURSOR_DEVIATION;
					mousePos *= SENSIVITY * k;

					XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mousePos.y), XMConvertToRadians(mousePos.x), 0);

					playerPhysicBody->mAngVelocity = playerPhysicBody->mAngVelocity * additionalRotation;
				}
			}

			if (mouse->IsLButtonClicked()) {
				comboManager->StartHeldInput(ComboInputType::Light);
			}

			if (mouse->IsRButtonClicked()) {
				comboManager->StartHeldInput(ComboInputType::Heavy);
			}

			if (mouse->IsLButtonUnclicked()) {
				comboManager->SaveInput(ComboInputType::Light);
			}

			if (mouse->IsRButtonUnclicked()) {
				comboManager->SaveInput(ComboInputType::Heavy);
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

	comboManager->Update();
}


void PlayerController::ProccessUI()
{
	TextLabel* elementText = elementLabel->GetComponent<TextLabel>();
	switch (abilities->element)
	{
		case Elements::None:
			elementText->textW = L"НЕТ";
			break;
		case Elements::Air:
			elementText->textW = L"ВОЗДУХ";
			break;
		case Elements::Fire:
			elementText->textW = L"ОГОНЬ";
			break;
		case Elements::Water:
			elementText->textW = L"ВОДА";
			break;
		case Elements::Earth:
			elementText->textW = L"ЗЕМЛЯ";
			break;
	}
}