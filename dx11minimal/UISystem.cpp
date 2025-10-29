#ifndef _UI_SYSTEM_
#define _UI_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Rect.cpp"

#include "cameraclass.h"


class UISystem : public System
{
public:
	UISystem()
	{
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
					Rect* rect = entity->GetComponent<Rect>();
					if (rect != nullptr)
					{
						Shaders::vShader(13);
						Shaders::pShader(13);
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