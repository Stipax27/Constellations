#ifndef _RENDER_SYSTEM_
#define _RENDER_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Sprite.cpp"
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
		Textures::RenderTarget(1, 0);

		// Clear the buffers to begin the scene.
		Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
		Draw::ClearDepth();

		Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
		Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::off);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();
		//m_Camera->AddRotation(0, 1, 0);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			Transform* transform = entity->GetComponent<Transform>();

			if (transform != nullptr)
			{

				XMMATRIX rotateMatrix, translateMatrix, scaleMatrix, srMatrix;
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

				XMVECTOR axis;
				float angle = transform->rotation.magnitude();
				if (angle > 0)
				{
					axis = XMVectorSet(transform->rotation.x / angle, transform->rotation.y / angle, transform->rotation.z / angle, 0.0f);
				}
				else
				{
					axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
				}

				rotateMatrix = XMMatrixRotationAxis(axis, angle);
				scaleMatrix = XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z);
				translateMatrix = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);

				// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
				srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
				XMMATRIX worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

				Sprite* sprite = entity->GetComponent<Sprite>();
				if (sprite != nullptr)
				{
					// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
				//sprite->model.Render(m_Direct3D->GetDeviceContext());

					Shaders::vShader(sprite->vShader);
					Shaders::pShader(sprite->pShader);

					ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, transform->scale.x);
					ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToVertex(5);

					Draw::Drawer(1);
				}
				
				Constellation* constellation = entity->GetComponent<Constellation>();
				if (constellation != nullptr)
				{
					// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
				//sprite->model.Render(m_Direct3D->GetDeviceContext());

					Shaders::vShader(4);
					Shaders::pShader(4);

					ConstBuf::global[2] = XMFLOAT4(1, 1, 1, 1);
					ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToPixel(5);

					for (int a = 0; a < constellation->links.size(); a++) {
						pair<int, int> link = constellation->links[a];
						point3d point1 = constellation->stars[link.first];
						point3d point2 = constellation->stars[link.second];

						ConstBuf::global[0] = XMFLOAT4(point1.x, point1.y, point1.z, 0.5);
						ConstBuf::global[1] = XMFLOAT4(point2.x, point2.y, point2.z, 0.5);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						Draw::Drawer(1);
					}

					Shaders::vShader(1);
					Shaders::pShader(1);

					for (int a = 0; a < constellation->stars.size(); a++) {
						point3d star = constellation->stars[a];

						ConstBuf::global[0] = XMFLOAT4(star.x, star.y, star.z, transform->scale.x);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						Draw::Drawer(1);
					}
				}
			}
		}

		Textures::CreateMipMap();
		Draw::OutputRenderTextures();

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