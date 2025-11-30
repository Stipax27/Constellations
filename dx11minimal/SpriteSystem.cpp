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
#include "PointCloud.cpp"

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
						int count;

						ConstBuf::global[constCount - 1] = XMFLOAT4(1, 1, 1, 1);

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
						Shaders::gShader(0);

						count = 0;
						for (int a = 0; a < constellation->links.size() && count < constCount / 2 - 1; a++) {
							pair<int, int> link = constellation->links[a];
							point3d point1 = transformedStars[link.first];
							point3d point2 = transformedStars[link.second];

							if (frustum->CheckSphere(point1.lerp(point2, 0.5f), max((point1 - point2).magnitude(), 0.25f))) {
								ConstBuf::global[count * 2] = XMFLOAT4(point1.x, point1.y, point1.z, 0.25f);
								ConstBuf::global[count * 2 + 1] = XMFLOAT4(point2.x, point2.y, point2.z, 0.25f);
								count++;
							}
						}

						if (count > 0) {
							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(5);

							context->DrawInstanced(6, min(count, constCount / 2 - 1), 0, 0);
						}

						Shaders::vShader(1);
						Shaders::pShader(1);
						Shaders::gShader(0);

						count = 0;
						for (int a = 0; a < transformedStars.size() && count < constCount - 1; a++) {
							point3d star = transformedStars[a];
							if (frustum->CheckSphere(star, transform->scale.x)) {
								ConstBuf::global[count] = XMFLOAT4(star.x, star.y, star.z, transform->scale.x);
								count++;
							}
						}

						if (count > 0) {
							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(5);

							context->DrawInstanced(6, min(count, constCount - 1), 0, 0);
						}
					}
				}

				SpriteCluster* spriteCluster = entity->GetComponent<SpriteCluster>();
				if (spriteCluster != nullptr) {
					ConstBuf::drawerV[0] = entity->timeScale;
					ConstBuf::Update(0, ConstBuf::drawerV);
					ConstBuf::ConstToVertex(0);
					ConstBuf::ConstToPixel(0);

					Shaders::vShader(spriteCluster->vShader);
					Shaders::pShader(spriteCluster->pShader);
					Shaders::gShader(0);

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

				PointCloud* pointCloud = entity->GetComponent<PointCloud>();
				if (pointCloud != nullptr) {
					Transform worldTransform = GetWorldTransform(entity);

					//if (frustum->CheckSphere(worldTransform.position, worldTransform.scale.magnitude())) {
						//ConstBuf::CreateVertexBuffer(15);

						XMMATRIX rotateMatrix = worldTransform.mRotation;
						XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
						XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

						// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
						XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
						XMMATRIX worldMatrix = srMatrix * translateMatrix;

						ConstBuf::camera.world = XMMatrixTranspose(worldMatrix);
						ConstBuf::UpdateCamera();

						ConstBuf::ConstToVertex(5);

						//Rasterizer::Cull(Rasterizer::cullmode::front);

						Shaders::vShader(17);
						Shaders::pShader(17);
						Shaders::gShader(17);

						InputAssembler::IA(InputAssembler::topology::triList);
						context->DrawIndexed(36, 0, 0);
					//}

				}
			}
		}
	}

private:
	FrustumClass* frustum;
};

#endif