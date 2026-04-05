#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include "../Utils/utils.h"
#include "../dx11.h"
#include "../../Point3d.h"
#include "../Lib/singleton.h"

#include "../../windowclass.h"
#include "../../frustumclass.h"

/////////////
// GLOBALS //
/////////////

#define DEFAULT_CAMERA_DISTANCE 7
#define DEFAULT_FOV 70
#define SCREEN_NEAR 0.1f
#define SCREEN_DEPTH 10000.0f

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass : public ISingleton
{
public:
	FrustumClass* frustum;
	float distance;

	point3d position;

public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void Initialize();

	void SetMatrixRotation(XMMATRIX);
	void SetFov(float);

	void AddMatrixRotation(XMMATRIX);
	void AddVectorRotation(point3d, float);

	XMMATRIX GetMatrixRotation();
	float GetFov();

	void Render();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

private:
	XMMATRIX rotationMatrix;
	float fov;
	float iaspect;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

private:
	void UpdateProjectionMatrix();
};

#endif