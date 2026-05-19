#ifndef _SKINNED_MESH_SYSTEM_H_
#define _SKINNED_MESH_SYSTEM_H_

#include "../../ECS_Base/system.h"
#include "../../Camera/frustumclass.h"
#include "../../Camera/cameraclass.h"

#include "../../Render//BoneAnimation/SkeletalAnimationComponent.h"
#include "SkinnedMesh.h"

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