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
	qRotation = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	iaspect = iAspect;

	SetFov(70);
}


void CameraClass::SetPosition(float x, float y, float z)
{
	position = point3d(x, y, z);
}


void CameraClass::SetEulerRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}


void CameraClass::SetQuaternionRotation(float x = 0.0f, float y = 1.0f, float z = 0.0f, float w = 0.0f)
{
	point3d rotation = quaternionToEuler(x, y, z, w);

	m_rotationX = rotation.x;
	m_rotationY = rotation.y;
	m_rotationZ = rotation.z;
}


void CameraClass::AddPosition(float x = 0.0f, float y = 0.0f, float z = 0.0f)
{
	position += point3d(x, y, z);
}


void CameraClass::AddEulerRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f)
{
	m_rotationX += x;
	m_rotationY += y;
	m_rotationZ += z;
}


void CameraClass::AddQuaternionRotation(float x = 0.0f, float y = 1.0f, float z = 0.0f, float w = 0.0f)
{
	point3d rotation = quaternionToEuler(x, y, z, w);

	m_rotationX += rotation.x;
	m_rotationY += rotation.y;
	m_rotationZ += rotation.z;
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


XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


void CameraClass::Render()
{
	XMFLOAT3 up, pos, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&pos);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * RAD;
	yaw = m_rotationY * RAD;
	roll = m_rotationZ * RAD;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	ConstBuf::camera.view = XMMatrixTranspose(m_viewMatrix);
	UpdateProjectionMatrix();

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