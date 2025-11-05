#ifndef _RENDER_SYSTEM_
#define _RENDER_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "SpriteCluster.cpp"
#include "Constellation.cpp"

#include "cameraclass.h"


class RenderSystem : public System
{
public:
	RenderSystem(CameraClass* camera)
	{
		m_Camera = camera;

		InputAssembler::IA(InputAssembler::topology::triList);
		Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
		Depth::Depth(Depth::depthmode::off);
		Rasterizer::Cull(Rasterizer::cullmode::off);
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
		if (m_Camera)
		{
			m_Camera = 0;
		}
	}


	bool Update(vector<Entity*>& entities, float deltaTime)
	{
		// Clear the buffers to begin the scene.
		Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
		Draw::ClearDepth();

		Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
		Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::off);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();
		//m_Camera->SetQuaternionRotation(0, 1, 0, timer::GetCounter() / 1000);
		//m_Camera->AddEulerRotation(0, 1, 0);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			if (entity->active)
			{
				Transform* transform = entity->GetComponent<Transform>();

				if (transform != nullptr)
				{
					bool result;

					//// Turn off the Z buffer to begin all 2D rendering.
					//m_Direct3D->TurnZBufferOff();

					//// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
					//result = m_Bitmap->Render(m_Direct3D->GetDeviceContext());
					//if (!result)
					//{
					//	return false;
					//}

					//// Render the bitmap with the UI shader.
					//result = m_ShaderManager->RenderUIShader(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), m_Bitmap->GetTexture());
					//if (!result)
					//{
					//	return false;
					//}

					//// Turn the Z buffer back on now that all 2D rendering has completed.
					//m_Direct3D->TurnZBufferOn();

					//rotateMatrix = XMMatrixRotationQuaternion(transform->qRotation);
					//scaleMatrix = XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z);
					//translateMatrix = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);

					// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
					//srMatrix = XMMatrixMultiply(scaleMatrix, transform->mRotation);
					//XMMATRIX worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

					Constellation* constellation = entity->GetComponent<Constellation>();
					if (constellation != nullptr)
					{
						// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
					//sprite->model.Render(m_Direct3D->GetDeviceContext());

						point3d transformPos = transform->position;
						vector<point3d> transformedStars;

						Shaders::vShader(4);
						Shaders::pShader(4);

						ConstBuf::global[2] = XMFLOAT4(1, 1, 1, 1);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToPixel(5);

						for (int a = 0; a < constellation->stars.size(); a++) {
							point3d star = constellation->stars[a];
							star = transformPos + transform->GetLookVector() * star.z + transform->GetRightVector() * star.x + transform->GetUpVector() * star.y;

							transformedStars.push_back(star);
						}

						for (int a = 0; a < constellation->links.size(); a++) {
							pair<int, int> link = constellation->links[a];
							point3d point1 = transformedStars[link.first];
							point3d point2 = transformedStars[link.second];

							ConstBuf::global[0] = XMFLOAT4(point1.x, point1.y, point1.z, 0.25);
							ConstBuf::global[1] = XMFLOAT4(point2.x, point2.y, point2.z, 0.25);
							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(5);

							Draw::Drawer(1);
						}

						Shaders::vShader(1);
						Shaders::pShader(1);

						for (int a = 0; a < transformedStars.size(); a++) {
							point3d star = transformedStars[a];

							ConstBuf::global[0] = XMFLOAT4(star.x, star.y, star.z, transform->scale.x);
							ConstBuf::Update(5, ConstBuf::global);
							ConstBuf::ConstToVertex(5);

							Draw::Drawer(1);
						}
					}
				}

				SpriteCluster* spriteCluster = entity->GetComponent<SpriteCluster>();
				if (spriteCluster != nullptr)
				{
					// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
				//sprite->model.Render(m_Direct3D->GetDeviceContext());

					Shaders::vShader(spriteCluster->vShader);
					Shaders::pShader(spriteCluster->pShader);

					if (transform != nullptr)
					{
						ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, transform->scale.x);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);
						ConstBuf::ConstToPixel(5);
					}

					context->DrawInstanced(6, spriteCluster->pointsNum, 0, 0);
				}
			}
		}

		Textures::CreateMipMap();
		Draw::SwitchRenderTextures();

		Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
		Depth::Depth(Depth::depthmode::off);
		Rasterizer::Cull(Rasterizer::cullmode::off);

		Shaders::vShader(10);
		Shaders::pShader(100);
		context->Draw(6, 0);

		return true;
	}

private:
	CameraClass* m_Camera;
};

#endif