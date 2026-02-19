#pragma once
#include "../system.h"
#include "../Engine/Mesh/SkinnedMesh.h"
#include "../frustumclass.h"
#include "../cameraclass.h"

class SkinnedMeshSystem : public System
{
public:
    SkinnedMeshSystem(
    FrustumClass* f,
    CameraClass* c,
    ID3D11Buffer* boneBuf);

    void Initialize() override;
    void Shutdown() override;
    void Update(std::vector<Entity*>& entities, float deltaTime) override;

private:
    ID3D11Buffer* boneBuffer;
    FrustumClass* frustum;
    CameraClass* camera;

    XMMATRIX GetWorldMatrix(Transform worldTransform);
    void UpdateWorldMatrix(Transform worldTransform);
};