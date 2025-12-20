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
#include "Star.cpp"
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
			if (IsEntityValid(entity))
			{
				Shaders::gShader(0);

				Transform* transform = entity->GetComponent<Transform>();

				if (transform != nullptr)
				{
					Transform worldTransform = GetWorldTransform(entity);

					Constellation* constellation = entity->GetComponent<Constellation>();
					if (constellation != nullptr) {
						point3d transformPos = transform->position;
						vector<point3d> transformedStars;
						int count;

						ConstBuf::global[constCount - 1] = XMFLOAT4(1, 1, 1, 1);

						for (int a = 0; a < constellation->stars.size(); a++) {
							point3d star = constellation->stars[a];
							star = transformPos + transform->GetLookVector() * star.z + transform->GetRightVector() * star.x + transform->GetUpVector() * star.y;

							transformedStars.push_back(star);
						}

						count = 0;
						for (int a = 0; a < constellation->links.size() && count < constCount / 2 - 1; a++) {
							pair<int, int> link = constellation->links[a];
							point3d point1 = transformedStars[link.first];
							point3d point2 = transformedStars[link.second];

							if (frustum->CheckSphere(point1.lerp(point2, 0.5f), max((point1 - point2).magnitude(), constellation->linkSize))) {
								ConstBuf::global[count * 2] = XMFLOAT4(point1.x, point1.y, point1.z, constellation->linkSize);
								ConstBuf::global[count * 2 + 1] = XMFLOAT4(point2.x, point2.y, point2.z, constellation->linkSize);
								count++;
							}
						}

						if (count > 0) {
							Shaders::vShader(4);
							Shaders::pShader(4);

							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(5);

							context->DrawInstanced(6, min(count, constCount / 2 - 1), 0, 0);
						}

						int n = 48;
						ConstBuf::drawerV[0] = n;

						count = 0;
						for (int a = 0; a < transformedStars.size() && count < constCount - 1; a++) {
							point3d star = transformedStars[a];
							if (frustum->CheckSphere(star, transform->scale.x)) {
								ConstBuf::global[count] = XMFLOAT4(star.x, star.y, star.z, constellation->starSize);
								count++;
							}
						}

						if (count > 0) {
							Shaders::vShader(20);
							Shaders::pShader(20);

							ConstBuf::Update(0, ConstBuf::drawerV);
							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(0);
							ConstBuf::ConstToVertex(5);

							context->DrawInstanced(n * 6, min(count, constCount - 1), 0, 0);
						}
					}

					Star* star = entity->GetComponent<Star>();
					if (star != nullptr) {
						if (frustum->CheckSphere(worldTransform.position, star->radius)) {
							ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, star->radius);
							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(5);

							Shaders::vShader(20);
							Shaders::pShader(20);

							int n = 48;
							ConstBuf::drawerV[0] = n;
							Draw::Drawer(n);
						}
					}
				}

				SpriteCluster* spriteCluster = entity->GetComponent<SpriteCluster>();
				if (spriteCluster != nullptr) {
					ConstBuf::drawerV[0] = entity->timeScale;
					ConstBuf::Update(0, ConstBuf::drawerV);
					ConstBuf::ConstToVertex(0);
					ConstBuf::ConstToPixel(0);

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

					if (spriteCluster->halfSizedRender) {
						int lastRT = Textures::currentRT;

						Textures::RenderTarget(4, 0);
						Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
						Draw::ClearDepth();



						//Depth::Depth(Depth::depthmode::on);

						Compute::Dispatch(0, lastRT, 3);
						Textures::TextureToShader(3, 0);

						//Depth::Depth(Depth::depthmode::off);



						//Textures::DepthTarget(lastRT, 0);

						/*Shaders::vShader(10);
						Shaders::pShader(101);
						context->PSSetShaderResources(0, 1, &Textures::Texture[lastRT].DepthResView);
						context->Draw(6, 0);*/

						//Depth::Depth(Depth::depthmode::readonly);

						Sampler::SamplerComp(0);

						Shaders::vShader(spriteCluster->vShader);
						Shaders::pShader(spriteCluster->pShader);
						context->DrawInstanced(6, spriteCluster->pointsNum, 0, 0);

						//Sampler::SamplerComp(0);
						Textures::CreateMipMap();

						Textures::RenderTarget(lastRT, 0);

						Textures::TextureToShader(4, 0, targetshader::pixel);

						Shaders::vShader(10);
						Shaders::pShader(100);
						context->Draw(6, 0);

						//Depth::Depth(Depth::depthmode::readonly);
					}
					else {
						Shaders::vShader(spriteCluster->vShader);
						Shaders::pShader(spriteCluster->pShader);

						context->DrawInstanced(6, spriteCluster->pointsNum, 0, 0);
					}
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