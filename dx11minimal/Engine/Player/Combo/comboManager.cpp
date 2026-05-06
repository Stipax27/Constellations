#include "comboManager.h"
#include "../../Lib/timer.h"

#include "../../BasicComponents/Transform2D.h"
#include "../../UI/Text/TextLabel.h"
#include "../../Compute/DelayedDestroy/DelayedDestroy.h"

#include "fistsCombos.h"

/////////////////////////////////////////////////////////////////////

void test(EntityStorage* entityStorage, Entity* player, const point3d& direction)
{
	Entity* entity = entityStorage->CreateEntity("DebugText");

	Transform2D* transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(0.8f, 0.65f, 0.0f);

	TextLabel* textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"КОМБО СРАБОТАЛО!";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 44;
	textLabel->fontScale = 0.40f;
	textLabel->letterSpacingPx = 1.0f;

	DelayedDestroy* delayedDestroy = entity->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 2000;
}

/////////////////////////////////////////////////////////////////////

void ComboManager::Initialize()
{
	entityStorage = Singleton::GetInstance<EntityStorage>();
	playerEntity = entityStorage->GetEntityByName("Player");
	abilities = Singleton::GetInstance<PlayerAbilities>();

	comboList = {

		///////////////////////////////////
		// Комбинации Лёгких ударов КУЛАКОВ
		///////////////////////////////////

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
			},
			fistsCombos::first_punch_l,
			true
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Light,
			},
			fistsCombos::first_punch_l,
			true
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Light,
			},
			fistsCombos::first_punch_l,
			true
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Dash,
				ComboInputType::Light

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::TakeBow
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::TakeSword
			},
			test,
			false
		),

		////////////////////////////////////
		// Комбинации Тяжёлых ударов КУЛАКОВ
		////////////////////////////////////

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Heavy
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Pause,
				ComboInputType::Heavy

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::Pause,
				ComboInputType::Heavy,
				ComboInputType::Pause,
				ComboInputType::Heavy,
				ComboInputType::Pause,
				ComboInputType::Heavy
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::HeavyHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Dash,
				ComboInputType::Heavy

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::TakeBow
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeFists,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::TakeSword
			},
			test,
			false
		),

		////////////////////////////////
		// Комбинации Лёгких ударов МЕЧА
		////////////////////////////////

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::Heavy
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::LightHeld,
				ComboInputType::LightHeld,
				ComboInputType::LightHeld,
				ComboInputType::LightHeld
				
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Light,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Dash,
				ComboInputType::Light

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::TakeBow
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::TakeFists
			},
			test,
			false
		),


		////////////////////////////////
		// Комбинации Тяжёлых ударов МЕЧА
		////////////////////////////////

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Heavy
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Pause,
				ComboInputType::HeavyHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::HeavyHeld,
				ComboInputType::HeavyHeld,
				ComboInputType::HeavyHeld,
				ComboInputType::HeavyHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Heavy,
				ComboInputType::HeavyHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Dash,
				ComboInputType::Heavy

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::TakeBow
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeSword,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::TakeFists
			},
			test,
			false
		),

		////////////////////////////////
		// Комбинации Лёгких ударов ЛУКА
		////////////////////////////////

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Light
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::LightHeld,
				ComboInputType::LightHeld,
				ComboInputType::HeavyHeld
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::LightHeld,
				ComboInputType::Pause,
				ComboInputType::LightHeld,
				ComboInputType::Pause,
				ComboInputType::LightHeld,
				ComboInputType::Pause,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Light,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Dash,
				ComboInputType::Light

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::TakeSword
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Light,
				ComboInputType::Heavy,
				ComboInputType::TakeFists
			},
			test,
			false
		),

		////////////////////////////////
		// Комбинации Тяжёлых ударов ЛУКА
		////////////////////////////////

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Heavy
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::HeavyHeld
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Heavy,
				ComboInputType::Heavy,
				ComboInputType::Pause,
				ComboInputType::LightHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::HeavyHeld,
				ComboInputType::Pause,
				ComboInputType::HeavyHeld,
				ComboInputType::Pause,
				ComboInputType::HeavyHeld,
				ComboInputType::Pause,
				ComboInputType::HeavyHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::LightHeld,
				ComboInputType::HeavyHeld

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Dash,
				ComboInputType::Heavy

			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::TakeSword
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::TakeBow,
				ComboInputType::Heavy,
				ComboInputType::Light,
				ComboInputType::TakeFists
			},
			test,
			false
		),


	};

	bufferUnlockTime = 0;
	heldInput = ComboInputType::Pause;

	maxComboLength = 0;
	for (Combo& combo : comboList) {
		maxComboLength = max(maxComboLength, combo.steps.size());
	}

#if COMBO_BUFFER_DEBUG
	bufferUi = entityStorage->CreateEntity("InputBufferUI");

	Transform2D* transform2D = bufferUi->AddComponent<Transform2D>();
	transform2D->position = point3d(0.75f, 0.6f, 0.0f);
#endif
}

/////////////////////////////////////////////////////////////////////

void ComboManager::SaveInput(ComboInputType input)
{
	if (timer::currentTime < bufferUnlockTime)
		return;

	int inputBufferSize = inputBuffer.size();

	if (inputBufferSize > 0) {
		if (input == ComboInputType::Pause) {
			if (inputBuffer.back() == ComboInputType::Pause) {
				ClearInputBuffer();
				return;
			}
		}
	}
	else {
		if ((int)input >= COMBO_INPUT_WEAPON_INDEX) {
			return;
		}
	}

	if (heldInput != ComboInputType::Pause) {
		if (timer::currentTime - heldInputTime >= COMBO_HELD_INPUT_START)
			input = ComboInputType((int)heldInput + 2);

		heldInput = ComboInputType::Pause;
	}

	if (inputBufferSize == 0) {
		ComboInputType weaponInput = GetCITforCurrWeapon();
		inputBuffer.push_back(weaponInput);
#if COMBO_BUFFER_DEBUG
		PrintComboInput(weaponInput);
#endif
	}

	inputBuffer.push_back(input);
	lastInputTime = timer::currentTime;

	if (input != ComboInputType::Pause)
		LockInputBuffer(COMBO_BUFFER_LOCK_TIME);

#if COMBO_BUFFER_DEBUG
	PrintComboInput(input);
#endif

	for (Combo& combo : comboList) {
		if (IsComboComplete(combo)) {
			combo.run(entityStorage, playerEntity, point3d());

			if (combo.expandable) {
				break;
			}
			else {
				ClearInputBuffer();
				return;
			}
		}
	}

	if (inputBuffer.size() >= maxComboLength) {
		ClearInputBuffer();
	}
}


void ComboManager::StartHeldInput(ComboInputType input)
{
	if ((int)input > 1)
		return;

	// It's a crutch. I don't want to create a separate flag for the state when no key is being held down.
	if (heldInput == ComboInputType::Pause) {
		heldInput = input;
		heldInputTime = timer::currentTime;
	}
}


void ComboManager::Update()
{
	if (heldInput != ComboInputType::Pause)
		return;

	if (inputBuffer.size() > 0 && timer::currentTime - lastInputTime >= COMBO_PAUSE_TIME) {
		SaveInput(ComboInputType::Pause);
	}
}


bool ComboManager::IsComboComplete(Combo& combo)
{
	return combo.steps == inputBuffer;
}


void ComboManager::ClearInputBuffer()
{
	inputBuffer.clear();

#if COMBO_BUFFER_DEBUG
	for (Entity* child : bufferUi->GetChildren()) {
		Transform2D* transform2D = child->GetComponent<Transform2D>();
		if (!transform2D || transform2D->position.x == -0.1f)
			continue;

		transform2D->position.x = -0.1f;

		DelayedDestroy* delayedDestroy = child->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 500;
	}
#endif
}


void ComboManager::LockInputBuffer(double time) {
	bufferUnlockTime = timer::currentTime + time;
}


ComboInputType ComboManager::GetCITforCurrWeapon() {
	return ComboInputType((int)abilities->weapon + COMBO_INPUT_WEAPON_INDEX);
}


#if COMBO_BUFFER_DEBUG
void ComboManager::PrintComboInput(const ComboInputType input)
{
	Entity* debugText = entityStorage->CreateEntity("InputText", bufferUi);

	Transform2D* transform2D = debugText->AddComponent<Transform2D>();
	transform2D->position = point3d(0.0f, (float)inputBuffer.size() * -0.05f, 0.0f);

	TextLabel* textLabel = debugText->AddComponent<TextLabel>();
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 44;
	textLabel->fontScale = 0.40f;
	textLabel->letterSpacingPx = 1.0f;

	switch (input)
	{
	case ComboInputType::Light:
		textLabel->textW = L"ЛА";
		break;
	case ComboInputType::Heavy:
		textLabel->textW = L"ТА";
		break;
	case ComboInputType::LightHeld:
		textLabel->textW = L"ЛА УДЕРЖ.";
		break;
	case ComboInputType::HeavyHeld:
		textLabel->textW = L"ТА УДЕРЖ.";
		break;
	case ComboInputType::Pause:
		textLabel->textW = L"ПАУЗА";
		break;
	case ComboInputType::Dash:
		textLabel->textW = L"РЫВОК";
		break;
	case ComboInputType::TakeFists:
		textLabel->textW = L"ВЫБОР КУЛАКИ";
		break;
	case ComboInputType::TakeSword:
		textLabel->textW = L"ВЫБОР МЕЧ";
		break;
	case ComboInputType::TakeBow:
		textLabel->textW = L"ВЫБОР ЛУК";
		break;
	}
}
#endif