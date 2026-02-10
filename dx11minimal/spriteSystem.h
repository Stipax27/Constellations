#ifndef _SPRITE_SYSTEM_H_
#define _SPRITE_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
#include "SpriteCluster.h"
#include "Constellation.h"
#include "Star.h"
#include "PointCloud.h"
#include "ParticleEmitter.h"
#include "Beam.h"

#include "frustumclass.h"
#include "Explosion.cpp" 


class SpriteSystem : public System
{
public:
	SpriteSystem(FrustumClass*);
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