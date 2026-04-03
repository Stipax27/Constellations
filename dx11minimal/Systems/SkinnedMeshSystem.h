#ifndef _SKINNED_MESH_SYSTEM_H_
#define _SKINNED_MESH_SYSTEM_H_

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
    void Update(EntityStorage& entityStorage, float deltaTime) override;

private:
    ID3D11Buffer* boneBuffer;
    FrustumClass* frustum;
    CameraClass* camera;

    void UpdateWorldMatrix(Transform worldTransform);
};

#endif