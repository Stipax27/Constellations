#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include "DirectXMath.h"
using namespace DirectX;

/////////////
// GLOBALS //
/////////////
const float RAD = 0.0174532925f;


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetFov(float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float fov;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
};

#endif