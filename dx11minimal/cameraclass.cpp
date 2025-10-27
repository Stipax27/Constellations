#include "cameraclass.h"

CameraClass::CameraClass()
{
}

CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::Initialize(float iAspect)
{
	position = point3d();
	qRotation = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0);

	rotationMatrix = XMMATRIX{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	iaspect = iAspect;

	SetFov(70);
}


void CameraClass::SetPosition(float x, float y, float z)
{
	position = point3d(x, y, z);
}


void CameraClass::SetEulerRotation(float x, float y, float z)
{
	qRotation = eulerToQuanternion(x * RAD, y * RAD, z * RAD);
}


void CameraClass::SetQuaternionRotation(float x = 0.0f, float y = 1.0f, float z = 0.0f, float w = 0.0f)
{
	qRotation = XMQuaternionNormalize(XMQuaternionRotationAxis(XMVectorSet(x, y, z, 0.0f), w * RAD));
}


void CameraClass::AddPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f)
{
	position += point3d(x, y, z);
}


void CameraClass::AddMatrixRotation(XMMATRIX Matrix)
{
	XMMATRIX result = Matrix * rotationMatrix;
	rotationMatrix = result;
}


void CameraClass::AddEulerRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f)
{
	XMVECTOR addRotation = eulerToQuanternion(x * RAD, y * RAD, z * RAD);
	qRotation = XMQuaternionNormalize(XMQuaternionMultiply(qRotation, addRotation));
}


void CameraClass::AddQuaternionRotation(float x = 0.0f, float y = 1.0f, float z = 0.0f, float w = 0.0f)
{
	XMVECTOR addRotation = XMQuaternionRotationAxis(XMVectorSet(x, y, z, 0.0f), w * RAD);
	qRotation = XMQuaternionNormalize(XMQuaternionMultiply(qRotation, addRotation));
}


void CameraClass::SetFov(float FoV)
{
	fov = FoV;
	UpdateProjectionMatrix();
}


XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(position.x, position.y, position.z);
}


XMFLOAT3 CameraClass::GetEulerRotation()
{
	point3d eRotation = quaternionToEuler(XMVectorGetX(qRotation), XMVectorGetY(qRotation), XMVectorGetZ(qRotation), XMVectorGetW(qRotation));
	return XMFLOAT3(eRotation.x, eRotation.y, eRotation.z);
}


XMVECTOR CameraClass::GetQuaternionRotation()
{
	return qRotation;
}


void CameraClass::Render()
{
	XMFLOAT3 up, pos, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	pos = XMFLOAT3(position.x, position.y, position.z);

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&pos);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	/*pitch = m_rotationx * rad;
	yaw = m_rotationy * rad;
	roll = m_rotationz * rad;*/

	// Create the rotation matrix from the yaw, pitch, and roll values.
	//rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	//rotationMatrix = XMMatrixRotationQuaternion(qRotation);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	ConstBuf::camera.view = XMMatrixTranspose(m_viewMatrix);

	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}


XMMATRIX CameraClass::GetViewMatrix()
{
	return m_viewMatrix;
}


XMMATRIX CameraClass::GetProjectionMatrix()
{
	return m_projectionMatrix;
}


float CameraClass::GetFov()
{
	return fov;
}


void CameraClass::UpdateProjectionMatrix()
{
	m_projectionMatrix = XMMatrixPerspectiveFovLH(
		degreesToRadians(fov),
		iaspect,
		0.1f,
		10000.0f
	);

	ConstBuf::camera.proj = XMMatrixTranspose(m_projectionMatrix);
}