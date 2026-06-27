#include "EditCameraController.h"

#include "../Engine/Lib/input.h"
#include "../Engine/Lib/timer.h"
#include "../Engine/Lib/logging.h"

#include "../GlobalConfigs.h"

#include <format>
#include <string>

#define UP_VECTOR point3d(0, 1, 0)


void EditCameraController::Initialize()
{
	camera = Singleton::GetInstance<CameraClass>();
	mouse = Singleton::GetInstance<MouseClass>();
	window = Singleton::GetInstance<WindowClass>();

	mouse->state = MouseState::Free;

	mousePos = point3d();
}


void EditCameraController::Shutdown()
{
	if (camera)
		camera = 0;

	if (mouse)
		mouse = 0;

	if (window)
		window = 0;
}


void EditCameraController::Update()
{
	ProcessRotation();
	ProcessPosition();
}


void EditCameraController::ProcessPosition()
{
	point3d velocity = point3d();
	XMMATRIX cameraMatrix = camera->GetMatrixRotation();

	point3d upVector = GetUpVectorFromMatrix(cameraMatrix);

	point3d lookVector = GetLookVectorFromMatrix(cameraMatrix);
	lookVector = (lookVector - upVector * lookVector.dot(upVector)).normalized();

	point3d rightVecttor = GetRightVectorFromMatrix(cameraMatrix);
	rightVecttor = (rightVecttor - upVector * rightVecttor.dot(upVector)).normalized();

	if (input::IsKeyDown('W')) {
		velocity += lookVector;
	}
	if (input::IsKeyDown('S')) {
		velocity += lookVector * -1;
	}
	if (input::IsKeyDown('A')) {
		velocity += rightVecttor * -1;
	}
	if (input::IsKeyDown('D')) {
		velocity += rightVecttor;
	}
	if (input::IsKeyDown('E')) {
		velocity += upVector;
	}
	if (input::IsKeyDown('Q')) {
		velocity += upVector * -1;
	}

	if (velocity.magnitude() > 0) {
		float flySpeed = input::IsKeyDown(VK_LSHIFT) ? CAMERA_FAST_FLY_SPEED : CAMERA_FLY_SPEED;
		camera->position += velocity.normalized() * flySpeed * timer::deltaTimeS;
	}
}

void EditCameraController::ProcessRotation()
{
	if (mouse->IsRButtonClicked()) {
		mousePos = mouse->pos;
		mouseAbsPos = mouse->absolutePos;
	}

	if (mouse->IsRButtonDown()) {
		point3d mPos = mouse->pos - mousePos;

		if (mPos.magnitude() < 0.001f)
			return;

		mPos.y *= -1;
		mPos *= MOUSE_SENSIVITY * 15;

		XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mPos.y), XMConvertToRadians(mPos.x), 0);

		XMMATRIX cameraMatrix = camera->GetMatrixRotation();
		cameraMatrix = additionalRotation * cameraMatrix;
		cameraMatrix = TransformMatrixToUpVector(cameraMatrix, UP_VECTOR);

		point3d cameraUpVector = point3d(cameraMatrix.r[1].m128_f32[0], cameraMatrix.r[1].m128_f32[1], cameraMatrix.r[1].m128_f32[2]).normalized();
		additionalRotation = XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(mPos.x), 0);

		if (abs(GetSignedAngleBetweenVectors(cameraUpVector, UP_VECTOR, true)) >= 85) {
			cameraMatrix = camera->GetMatrixRotation();
			cameraMatrix = additionalRotation * cameraMatrix;
			cameraMatrix = TransformMatrixToUpVector(cameraMatrix, UP_VECTOR);
		}

		point3d cameraLookVector = point3d(cameraMatrix.r[2].m128_f32[0], cameraMatrix.r[2].m128_f32[1], cameraMatrix.r[2].m128_f32[2]).normalized();

		camera->SetMatrixRotation(cameraMatrix);

		mouse->absolutePos = mouseAbsPos;
		mouse->pos = mousePos;
		SetCursorPos(mouseAbsPos.x, mouseAbsPos.y);
	}
}