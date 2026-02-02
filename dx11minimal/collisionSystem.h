#ifndef _COLLISION_SYSTEM_H_
#define _COLLISION_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include <map>
#include <unordered_map>
#include <typeindex>
#include <functional>

#include<cmath>
#include "system.h"
#include "Transform.h"
#include "PhysicBody.h"

#include "collider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "SurfaceCollider.h"

#include "Health.h"
#include "SingleDamager.h"
#include "MultiDamager.h"

#include "MethodOfClosest.h"
#include "CollisionManagerClass.h"


class CollisionSystem : public System
{
public:
	CollisionSystem(CollisionManagerClass*);
	void Initialize();
	void Shutdown();

	//float f(float x);

	void Update(vector<Entity*>&, float);

private:
	CollisionManagerClass* collisionManager;
};

#endif