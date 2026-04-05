#ifndef _MESH_SYSTEM_H_
#define _MESH_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "Mesh.h"
#include "../../../Constellation.h"
#include "../../../Star.h"

#include "../../Camera/frustumclass.h"
#include "../../Camera/cameraclass.h"

/////////////
// GLOBALS //
/////////////
const bool SHOW_GRID = false;


class MeshSystem : public System
{
public:
	MeshSystem();
	void Initialize();
	void Shutdown();


	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;
	CameraClass* camera;

private:
	void UpdateWorldMatrix(Transform);
	int GetVertexCount(point3d, int, int, float);
};

#endif