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
	}


	bool Update(vector<Entity*>& entities, float deltaTime)
	{
		Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
		Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::off);

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

						ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, 0);
						ConstBuf::global[1] = XMFLOAT4(transform->scale.x, transform->scale.y, 0, 0);
						ConstBuf::global[2] = XMFLOAT4(rect->anchorPoint.x, rect->anchorPoint.y, 0, 0);
						ConstBuf::global[3] = XMFLOAT4(rect->color.x, rect->color.y, rect->color.z, 0);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);
						ConstBuf::ConstToPixel(5);

						Draw::Drawer(1);
					}
				}
			}
		}

		return true;
	}
};

#endif