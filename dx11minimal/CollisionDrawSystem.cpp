#ifndef _DEBUG_COLLISION_DRAW_SYSTEM_
#define _DEBUG_COLLISION_DRAW_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"

#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "SurfaceCollider.cpp"

#include "cameraclass.h"

/////////////
// GLOBALS //
/////////////
const bool DEPTH_OFF = true;


class CollisionDrawSystem : public System
{
public:
	CollisionDrawSystem()
	{
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
	}


	void Update(vector<Entity*>& entities, float deltaTime)
	{
		Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
		Rasterizer::Cull(Rasterizer::cullmode::wireframe);

		if (DEPTH_OFF) {
			Depth::Depth(Depth::depthmode::off);
		}
		else {
			Depth::Depth(Depth::depthmode::readonly);
		}

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Shaders::gShader(0);

			Entity* entity = entities[i];
			if (IsEntityValid(entity))
			{
				Transform* transform = entity->GetComponent<Transform>();

				if (transform != nullptr)
				{
					Transform worldTransform = GetWorldTransform(entity);

					ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, 0);

					SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();
					if (sphereCollider != nullptr) {
						ConstBuf::global[0].w = sphereCollider->radius;
						ConstBuf::global[1] = XMFLOAT4(1, 0, 0, 0);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						Shaders::vShader(18);
						Shaders::pShader(18);

						int n = 11 * sqrt(sphereCollider->radius);
						ConstBuf::drawerV[0] = n;
						Draw::Drawer(n * n);
					}

					/*PlaneCollider* planeCollider = entity->GetComponent<PlaneCollider>();
					if (planeCollider != nullptr) {
						Shaders::vShader(18);
						Shaders::pShader(18);

						int n = 11;
						ConstBuf::drawerV[0] = n;
						Draw::Drawer(n * n);
					}*/
				}
			}
		}
	}
};

#endif