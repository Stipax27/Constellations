#include "Skeleton.h"
#include <functional>
using namespace DirectX;

void Skeleton::UpdateGlobalPose()
{
	if (joints.empty())
		return;

	std::vector<char> resolved(joints.size(), 0);
	std::vector<char> inStack(joints.size(), 0);

	std::function<void(size_t)> resolveJoint = [&](size_t idx)
	{
		if (idx >= joints.size())
			return;

		if (resolved[idx])
			return;

		// Guard against malformed cyclic hierarchies.
		if (inStack[idx])
		{
			joints[idx].global = joints[idx].local;
			resolved[idx] = 1;
			return;
		}

		inStack[idx] = 1;

		const int parentIdx = joints[idx].parent;
		if (parentIdx < 0 || static_cast<size_t>(parentIdx) >= joints.size())
		{
			joints[idx].global = joints[idx].local;
		}
		else
		{
			resolveJoint(static_cast<size_t>(parentIdx));

			const XMMATRIX parentGlobal = XMLoadFloat4x4(&joints[parentIdx].global);
			const XMMATRIX local = XMLoadFloat4x4(&joints[idx].local);
			// Row-vector convention (mul(pos, M)): child global = local * parent.
			XMStoreFloat4x4(&joints[idx].global, local * parentGlobal);
		}

		inStack[idx] = 0;
		resolved[idx] = 1;
	};

	for (size_t i = 0; i < joints.size(); ++i)
	{
		resolveJoint(i);
	}
}