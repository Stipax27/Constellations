#ifndef _RENDER_SYSTEM_
#define _RENDER_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Sprite.cpp"

#include "cameraclass.h"

#include "dx11.h"

using namespace DirectX;


class RenderSystem : public System
{
public:
	RenderSystem(CameraClass* camera)
	{
		m_Camera = camera;
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
		XMMATRIX viewMatrix, projectionMatrix, orthoMatrix;

		// Clear the buffers to begin the scene.
		Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
		Draw::ClearDepth();

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_Camera->GetViewMatrix(viewMatrix);
		m_Direct3D->GetProjectionMatrix(projectionMatrix);
		m_Direct3D->GetOrthoMatrix(orthoMatrix);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			Transform* transform = entity->GetComponent<Transform>();

			Sprite* sprite = entity->GetComponent<Sprite>();
			if (transform != nullptr && sprite != nullptr)
			{

				XMMATRIX worldMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix;
				bool result;

				m_Direct3D->GetWorldMatrix(worldMatrix);

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
				worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

				// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
				//sprite->model.Render(m_Direct3D->GetDeviceContext());

				Shaders::vShader(sprite->vShader);
				Shaders::pShader(sprite->pShader);

				// Render the model using shader.
				result = m_ShaderManager->RenderShader(m_Direct3D->GetDeviceContext(), mesh->model.GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, mesh->model.GetTexture(), m_Camera->GetPosition());
				if (!result)
				{
					return false;
				}

			}
		}

		// Present the rendered scene to the screen.
		Draw::Present();

		return true;
	}

private:
	CameraClass* m_Camera;
};

#endif