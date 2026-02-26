#ifndef _NEBULA_SYSTEM_H_
#define _NEBULA_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
#include "Components/Nebula.h"

#include "frustumclass.h"


class NebulaSystem : public System
{
public:
	NebulaSystem(FrustumClass*);
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);

private:
	FrustumClass* frustum;

private:
	XMMATRIX GetWorldMatrix(Transform);
	void UpdateWorldMatrix(Transform);
};

#endif