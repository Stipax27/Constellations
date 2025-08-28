

namespace Hero
{
	struct State
	{
		float timeShiftPressed;

		XMVECTOR relativeMovement = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR currentRotation = XMQuaternionIdentity();
		XMVECTOR rotationBeforeAttack = XMQuaternionIdentity();
		XMVECTOR targetRotation = XMQuaternionIdentity();
		XMVECTOR Forwardbuf = XMVectorSet(0, 0, 1, 0);
		XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
		XMVECTOR Right = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR at = XMVectorSet(0, 0, 0, 0);
		XMVECTOR Up = XMVector3Rotate(defaultUp, currentRotation);

		XMMATRIX constellationOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX constellationSubOffset = XMMatrixTranslation(0, 0, 0);

		XMVECTOR position = XMVectorSet(0, 0, 0, 0);
		XMMATRIX worldMatrix = XMMatrixIdentity();

		XMMATRIX meshRotationMatrix = XMMatrixIdentity();
		float meshRotationAngle = 0.0f;
		float meshRotationSpeed = .3f;
		bool isMeshRotating = false;

		HeroAnimations::QuaternionState* animationState;
		

		XMVECTOR animationStartRotation = XMQuaternionIdentity();
		XMVECTOR animationTargetRotation = XMQuaternionIdentity();
		float animationProgress = 0.0f;
		bool isAnimating = false;
		DWORD animationStartTime = 0;
		float animationDuration = 1.f;

		float localRotationSmoothness = 0.05f;
		float attackRotationProgress = 0.0f;
		XMVECTOR localRotationVelocity = XMVectorZero();
		XMMATRIX rotationMatrix = XMMatrixIdentity();
		bool isAttackRotating = false;
		float rotationSpeed = .3f;

		// Для нелинейной интерполяции
		float rotationSmoothFactor = 0.1f;
		float rotationAcceleration = 2.0f;
		float rotationDeceleration = 3.0f;
		XMVECTOR rotationVelocity = XMVectorZero();

		State() {
			// Создаем анимационное состояние при инициализации
			animationState = new HeroAnimations::QuaternionState();
			// Синхронизируем начальные значения
			animationState->syncFromXMVectors(currentRotation, rotationBeforeAttack, targetRotation);
		}

		~State() {
			delete animationState;
		}
	}static state;



	void StartRotationAnimation(float yawDegrees, float pitchDegrees, float rollDegrees) {
		HeroAnimations::AnimationHelpers::StartRotationAnimation(
			*state.animationState, yawDegrees, pitchDegrees, rollDegrees);

		// Устанавливаем флаги анимации
		state.animationProgress = 0.0f;
		state.isAnimating = true;
		state.animationStartTime = GetTickCount();
	}

	void UpdateRotationAnimation(float deltaTime) {
		HeroAnimations::AnimationHelpers::UpdateRotationAnimation(
			*state.animationState, state, deltaTime);
	}

	void RotateTowardsTarget(XMVECTOR targetDirection, float deltaTime) {
		HeroAnimations::AnimationHelpers::RotateTowardsTarget(
			*state.animationState, state, targetDirection, deltaTime);
	}
}
