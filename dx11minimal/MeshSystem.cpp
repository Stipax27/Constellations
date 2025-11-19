#ifndef _MESH_SYSTEM_
#define _MESH_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Mesh.cpp"

#include "cameraclass.h"


class MeshSystem : public System
{
public:
	MeshSystem()
	{
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
	}


	bool Update(vector<Entity*>& entities, float deltaTime)
	{
		// Clear the buffers to begin the scene.
		/*Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
		Draw::ClearDepth();*/

		Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
		//Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
		//Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::on);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			if (entity->IsActive())
			{
				Transform* transform = entity->GetComponent<Transform>();

				if (transform != nullptr)
				{
					Mesh* mesh = entity->GetComponent<Mesh>();
					if (mesh != nullptr) {
						ConstBuf::CreateVertexBuffer(15);

						ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, 0);
						ConstBuf::global[1] = XMFLOAT4(transform->scale.x, transform->scale.y, transform->scale.z, 0);
						ConstBuf::Update(5, ConstBuf::global);

						transform->mRotation = XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), 1 * RAD) * transform->mRotation;

						XMMATRIX rotateMatrix = transform->mRotation;
						XMMATRIX scaleMatrix = XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z);
						XMMATRIX translateMatrix = XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);

						// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
						XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
						XMMATRIX worldMatrix = srMatrix * translateMatrix;

						ConstBuf::drawerMat.model = XMMatrixTranspose(worldMatrix);
						ConstBuf::UpdateDrawerMat();

						ConstBuf::ConstToVertex(5);

						Rasterizer::Cull(mesh->cullMode);

						Shaders::vShader(15);
						Shaders::pShader(15);
						InputAssembler::IA(InputAssembler::topology::triList);
						context->DrawIndexed(36, 0, 0);
					}
				}
			}
		}

		return true;
	}
};

#endif