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
	void SetMatrixRotation(XMMATRIX);
	void SetFov(float);

	void AddPosition(float, float, float);
	void AddMatrixRotation(XMMATRIX);
	void AddVectorRotation(point3d, float);

	point3d GetPosition();
	XMMATRIX GetMatrixRotation();
	float GetFov();

	void Render();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

private:
	point3d position;
	XMMATRIX rotationMatrix;
	float fov;
	float iaspect;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

private:
	void UpdateProjectionMatrix();
};

#endif