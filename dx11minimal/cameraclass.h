#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include "utils.h"
#include "dx11.h"
#include "Point3d.h"


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

	void SetPosition(point3d);
	void SetEulerRotation(float, float, float);
	void SetQuaternionRotation(float, float, float, float);
	void SetFov(float);

	void AddPosition(float, float, float);
	void AddMatrixRotation(XMMATRIX);
	void AddEulerRotation(float, float, float);
	void AddVectorRotation(point3d, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetEulerRotation();
	XMVECTOR GetQuaternionRotation();
	float GetFov();

	void Render();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

private:
	point3d position;
	XMVECTOR qRotation;
	float fov;
	float iaspect;

	XMMATRIX rotationMatrix;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

private:
	void UpdateProjectionMatrix();
};

#endif