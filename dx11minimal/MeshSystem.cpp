#ifndef _MESH_SYSTEM_
#define _MESH_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Mesh.cpp"

#include "frustumclass.h"


class MeshSystem : public System
{
public:
	MeshSystem(FrustumClass* Frustum)
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

		Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
		//Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::on);

		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			if (IsEntityValid(entity))
			{
				Transform* transform = entity->GetComponent<Transform>();

				if (transform != nullptr)
				{
					Mesh* mesh = entity->GetComponent<Mesh>();
					if (mesh != nullptr) {
						Transform worldTransform = GetWorldTransform(entity);

						if (entity->parent != nullptr && !entity->parent->HasComponent<Transform>()) {
							transform->mRotation = XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), -2 * RAD) * transform->mRotation;
						}

						if (frustum->CheckSphere(worldTransform.position, worldTransform.scale.magnitude())) {
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

							Rasterizer::Cull(mesh->cullMode);

							Shaders::vShader(15);
							Shaders::pShader(15);
							Shaders::gShader(0);

							InputAssembler::IA(InputAssembler::topology::triList);
							context->DrawIndexed(36, 0, 0);
						}

					}
				}
			}
		}
	}

private:
	FrustumClass* frustum;
};

#endif