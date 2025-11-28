#ifndef _SPRITE_SYSTEM_
#define _SPRITE_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "SpriteCluster.cpp"
#include "Constellation.cpp"

#include "frustumclass.h"


class SpriteSystem : public System
{
public:
	SpriteSystem(FrustumClass* Frustum)
	{
		frustum = Frustum;
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
	}


	void Update(vector<Entity*>& entities, float deltaTime)
	{
		// Clear the buffers to begin the scene.
		/*Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
		Draw::ClearDepth();*/

		Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
		Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::readonly);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			if (entity->IsActive())
			{
				Transform* transform = entity->GetComponent<Transform>();

				if (transform != nullptr)
				{
					Constellation* constellation = entity->GetComponent<Constellation>();
					if (constellation != nullptr) {
						// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
					//sprite->model.Render(m_Direct3D->GetDeviceContext());

						point3d transformPos = transform->position;
						vector<point3d> transformedStars;


						////debug point
						//Shaders::vShader(1);
						//Shaders::pShader(1);

						//point3d closestPoint;
						//float distance;
						//SurfaceCollider surface;

						//bool collision = findClosestPointOnSurface(transformPos, surface, closestPoint, distance, 12, 3);

						//ConstBuf::global[0] = XMFLOAT4(closestPoint.x, closestPoint.y, closestPoint.z, 5.0f);
						//ConstBuf::Update(5, ConstBuf::global);
						//ConstBuf::ConstToVertex(5);

						//Draw::Drawer(1);

						for (int a = 0; a < constellation->stars.size(); a++) {
							point3d star = constellation->stars[a];
							star = transformPos + transform->GetLookVector() * star.z + transform->GetRightVector() * star.x + transform->GetUpVector() * star.y;

							transformedStars.push_back(star);
						}

						Shaders::vShader(4);
						Shaders::pShader(4);

						for (int a = 0; a < constellation->links.size() && a < constCount / 2 - 1; a++) {
							pair<int, int> link = constellation->links[a];
							point3d point1 = transformedStars[link.first];
							point3d point2 = transformedStars[link.second];

							ConstBuf::global[a * 2] = XMFLOAT4(point1.x, point1.y, point1.z, 0.25f);
							ConstBuf::global[a * 2 + 1] = XMFLOAT4(point2.x, point2.y, point2.z, 0.25f);
						}

						ConstBuf::global[constCount - 1] = XMFLOAT4(1, 1, 1, 1);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						context->DrawInstanced(6, min(constellation->links.size(), constCount / 2 - 1), 0, 0);

						Shaders::vShader(1);
						Shaders::pShader(1);

						for (int a = 0; a < transformedStars.size() && a < constCount - 1; a++) {
							point3d star = transformedStars[a];
							ConstBuf::global[a] = XMFLOAT4(star.x, star.y, star.z, transform->scale.x);
						}

						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						context->DrawInstanced(6, min(transformedStars.size(), constCount - 1), 0, 0);
					}
				}

				SpriteCluster* spriteCluster = entity->GetComponent<SpriteCluster>();
				if (spriteCluster != nullptr)
				{
					ConstBuf::drawerV[0] = entity->timeScale;
					ConstBuf::Update(0, ConstBuf::drawerV);
					ConstBuf::ConstToVertex(0);
					ConstBuf::ConstToPixel(0);

					Shaders::vShader(spriteCluster->vShader);
					Shaders::pShader(spriteCluster->pShader);

					if (transform != nullptr)
					{
						/*if (!frustum->CheckSphere(transform->position, transform->scale.x)) {
							continue;
						}*/

						ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, transform->scale.x);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);
						ConstBuf::ConstToPixel(5);
					}

					context->DrawInstanced(6, spriteCluster->pointsNum, 0, 0);
				}
			}
		}
	}

private:
	FrustumClass* frustum;
};

#endif