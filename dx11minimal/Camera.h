#include "dx11.h"

namespace Camera
{
	struct State
	{
		int n = 0;
		bool mouse = false;
		float camDist = 500.0f;
		float minDist = 500.0f;
		float maxDist = 1000.0f;
		float fovAngle = 60.0f;
		float verticalOffset = 180.0f;
		float horizontalOffset = 0.0f;
		float distanceOffset = 600.0f;

		bool isDetached = false;
		XMVECTOR detachedPosition;
		XMVECTOR detachedTarget;
		XMVECTOR detachedUp;
		XMVECTOR heroPositionAtDetach;
		XMVECTOR relativeMovement = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR currentRotation = XMQuaternionIdentity();
		XMVECTOR Forward = XMVectorSet(0, 0, 1, 0);
		XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
		XMVECTOR Right = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR at = XMVectorSet(0, 0, 0, 0);
		XMVECTOR Up = XMVector3Rotate(defaultUp, currentRotation);
		XMVECTOR Eye;
		XMMATRIX constellationOffset = XMMatrixTranslation(0, 0, 0);
		XMVECTOR EyeBack = XMVectorSet(0, 0, -1, 0);


		float cameraSmoothness = 0.1f;
		XMVECTOR cameraRotationVelocity = XMVectorZero();

	} static state;

	void DetachCamera()
	{
		if (!state.isDetached)
		{
			state.isDetached = true;
			state.detachedPosition = state.Eye;
			state.detachedTarget = state.at;
			state.detachedUp = state.Up;
			state.heroPositionAtDetach = Hero::state.position;
		}
	}

	void AttachCamera()
	{
		if (state.isDetached)
		{
			state.isDetached = false;
		}
	}

	// ѕлавное вращение камеры с использованием кватернионов
	void SmoothRotateCamera(XMVECTOR targetRotation, float deltaTime)
	{

		state.Up = XMVector3Rotate(state.defaultUp, state.currentRotation);
		state.Forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), state.currentRotation);
		state.Right = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), state.currentRotation);
	}

	void UpdateCameraPosition()
	{
		if (state.isDetached)
		{
			XMVECTOR heroPositionDelta = Hero::state.position - state.heroPositionAtDetach;
			state.Eye = state.detachedPosition + heroPositionDelta;
			state.at = state.detachedTarget + heroPositionDelta;
			state.heroPositionAtDetach = Hero::state.position;

			ConstBuf::camera.view[0] = XMMatrixTranspose(
				XMMatrixLookAtLH(state.Eye, state.at, state.Up)
			);
			return;
		}

		// ѕлавное следование за вращением геро€
		XMVECTOR targetCameraRotation = Hero::state.currentRotation;
		SmoothRotateCamera(targetCameraRotation, 0.016f); // ѕример deltaTime

		XMVECTOR heroForward = XMVector3Normalize(Hero::state.Forwardbuf);
		XMVECTOR heroUp = XMVector3Normalize(Hero::state.Up);
		XMVECTOR heroRight = XMVector3Normalize(Hero::state.Right);

		XMVECTOR cameraOffset =
			(-heroForward * state.distanceOffset) +
			(heroUp * state.verticalOffset) +
			(heroRight * state.horizontalOffset);

		XMVECTOR cameraPosition = Hero::state.position + cameraOffset;
		XMVECTOR cameraTarget = cameraPosition + (heroForward * 15.0f);

		state.Eye = cameraPosition;
		state.at = cameraTarget;
		state.Up = heroUp;

		state.Forward = XMVector3Normalize(cameraTarget - cameraPosition);
		state.Right = XMVector3Normalize(XMVector3Cross(heroUp, state.Forward));

		ConstBuf::camera.view[0] = XMMatrixTranspose(
			XMMatrixLookAtLH(state.Eye, state.at, state.Up)
		);
	}

	void Camera()
	{
		UpdateCameraPosition();

		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(
			XMConvertToRadians(state.fovAngle),
			iaspect,
			0.01f,
			10000.0f
		));

		ConstBuf::UpdateCamera();
		ConstBuf::ConstToVertex(3);
		ConstBuf::ConstToPixel(3);
	}

	void HandleMouseWheel(int delta)
	{
		// Ќелинейное изменение рассто€ни€ с плавным ускорением
		float speedFactor = 1.0f + abs(delta) * 0.1f;
		state.distanceOffset -= delta * 5.0f * speedFactor;
		state.distanceOffset = max(state.minDist, min(state.distanceOffset, state.maxDist));
	}
}
