#include "comboManager.h"
#include "../../Lib/timer.h"

#include "../../BasicComponents/Transform2D.h"
#include "../../UI/Text/TextLabel.h"
#include "../../Compute/DelayedDestroy/DelayedDestroy.h"

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

	comboList = {
		Combo
		(
			{
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Heavy,
			},
			test,
			false
		),

		Combo
		(
			{
				ComboInputType::Light,
				ComboInputType::Light,
				ComboInputType::Pause,
				ComboInputType::Heavy,
			},
			test,
			false
		)
	};

	bufferUnlockTime = 0;

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

	if (input == ComboInputType::Pause && inputBuffer.size() > 0) {
		if (inputBuffer.back() == ComboInputType::Pause) {
			ClearInputBuffer();
			return;
		}
	}

	inputBuffer.push_back(input);
	lastInputTime = timer::currentTime;
	LockInputBuffer(COMBO_BUFFER_LOCK_TIME);

#if COMBO_BUFFER_DEBUG
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
	case ComboInputType::SwitchToFists:
		textLabel->textW = L"С КУЛАКИ";
		break;
	case ComboInputType::SwitchToSword:
		textLabel->textW = L"С МЕЧ";
		break;
	case ComboInputType::SwitchToBow:
		textLabel->textW = L"С ЛУК";
		break;
	}
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


void ComboManager::Update()
{
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
		if (!transform2D)
			continue;

		transform2D->position.x -= 0.1f;

		DelayedDestroy* delayedDestroy = child->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 500;
	}
#endif
}


void ComboManager::LockInputBuffer(double time) {
	bufferUnlockTime = timer::currentTime + time;
}