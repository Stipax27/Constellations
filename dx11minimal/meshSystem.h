#ifndef _MESH_SYSTEM_H_
#define _MESH_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
#include "Mesh.h"
#include "Constellation.h"
#include "Star.h"

#include "frustumclass.h"
#include "cameraclass.h"


class MeshSystem : public System
{
public:
	MeshSystem(FrustumClass*, CameraClass*);
	void Initialize();
	void Shutdown();


	void Update(vector<Entity*>&, float);

private:
	FrustumClass* frustum;
	CameraClass* camera;

private:
	XMMATRIX GetWorldMatrix(Transform);
	void UpdateWorldMatrix(Transform);
	int GetVertexCount(point3d, int, int);
};

#endif