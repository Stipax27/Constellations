#ifndef _POINT_CLOUD_SYSTEM_H_
#define _POINT_CLOUD_SYSTEM_H_

#include <vector>

#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"
#include "../Camera/frustumclass.h"
#include "../Render/BoneAnimation/SkeletalAnimationComponent.h"
#include "PointCloud.h"

class PointCloudSystem : public System
{
public:
	PointCloudSystem();
	PointCloudSystem(FrustumClass*, ID3D11Buffer* boneBuf = nullptr);

	void Initialize();
	void Shutdown();
	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;
	ID3D11Buffer* boneBuffer = nullptr;

private:
	Transform GetPointCloudTransform(Entity*, PointCloud*);
	void RenderMeshPointCloud(Entity*, PointCloud*, const Transform&);
	void RenderProceduralPointCloud(PointCloud*, const Transform&);
	void RenderProceduralPass(PointCloud*, int, int, bool);
	void ClearProceduralTargets();
	void CompositeProceduralTargets(int);
	int GetProceduralTarget(pMode);
	void UpdateWorldMatrix(Transform);
	void BindBones(Entity*);
	void PSModeSet(pMode, bool);
};

#endif
