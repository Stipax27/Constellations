#ifndef _SKINNED_MESH_SYSTEM_H_
#define _SKINNED_MESH_SYSTEM_H_

#include "../Engine/ECS_Base/system.h"
#include "../Engine/Mesh/SkinnedMesh.h"
#include "../Engine/Camera/frustumclass.h"
#include "../Engine/Camera/cameraclass.h"

#include "../Engine/Render//BoneAnimation/SkeletalAnimationComponent.h"

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