#ifndef _RENDER_SYSTEM_
#define _RENDER_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Sprite.cpp"

#include "d3dclass.h"
#include "cameraclass.h"
#include "lightclass.h"
#include "pointlightclass.h"

#include "DirectXMath.h"

using namespace DirectX;

/////////////
// GLOBALS //
/////////////
const float PI = 3.14159265358979f;


class RenderSystem : public System
{
public:
	RenderSystem(D3DClass* direct3d, CameraClass* camera, LightClass* light, PointLightClass* pointLights, int numPointLights)
	{
		m_Direct3D = direct3d;
		m_Camera = camera;
		m_Light = light;
		m_PointLights = pointLights;
		m_numPointLights = numPointLights;
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
		if (m_Direct3D)
		{
			m_Direct3D = 0;
		}

		if (m_Camera)
		{
			m_Camera = 0;
		}

		if (m_Light)
		{
			m_Light = 0;
		}

		if (m_PointLights)
		{
			m_PointLights = 0;
		}

		if (m_numPointLights)
		{
			m_numPointLights = 0;
		}
	}


	bool Update(vector<Entity*>& entities, float deltaTime)
	{
		// Clear the buffers to begin the scene.
		m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			Transform* transform = entity->GetComponent<Transform>();

			Mesh* mesh = entity->GetComponent<Mesh>();
			if (transform != nullptr && mesh != nullptr)
			{

				XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix, orthoMatrix;
				XMFLOAT4 pointLightColor[4], pointLightPosition[4];
				int i;
				bool result;

				// Generate the view matrix based on the camera's position.
				m_Camera->Render();

				// Get the world, view, and projection matrices from the camera and d3d objects.
				m_Direct3D->GetWorldMatrix(worldMatrix);
				m_Camera->GetViewMatrix(viewMatrix);
				m_Direct3D->GetProjectionMatrix(projectionMatrix);
				m_Direct3D->GetOrthoMatrix(orthoMatrix);

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
				mesh->model.Render(m_Direct3D->GetDeviceContext());

				// Get the light properties.
				for (i = 0; i < m_numPointLights; i++)
				{
					// Create the diffuse color array from the four light colors.
					pointLightColor[i] = m_PointLights[i].GetDiffuseColor();

					// Create the light position array from the four light positions.
					pointLightPosition[i] = m_PointLights[i].GetPosition();
				}

				// Render the model using shader.
				result = m_ShaderManager->RenderShader(m_Direct3D->GetDeviceContext(), mesh->model.GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, mesh->model.GetTexture(),
					m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
					m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(),
					pointLightColor, pointLightPosition);
				if (!result)
				{
					return false;
				}

			}
		}

		// Present the rendered scene to the screen.
		m_Direct3D->EndScene();

		return true;
	}

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	LightClass* m_Light;
	PointLightClass* m_PointLights;
	int m_numPointLights;
};

#endif