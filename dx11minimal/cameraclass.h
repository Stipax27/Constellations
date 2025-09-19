#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include "utils.h"
#include "dx11.h"

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

	void Initialize(float);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetFov(float);

	void AddPosition(float, float, float);
	void AddRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	float GetFov();

	void Render();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float fov;
	float iaspect;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

private:
	void UpdateProjectionMatrix();
};

#endif