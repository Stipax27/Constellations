#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

struct Joint
{
	std::string name;
	int parent = -1;
	std::vector<int> children;

	DirectX::XMFLOAT4X4 local;
	DirectX::XMFLOAT4X4 global;
	DirectX::XMFLOAT4X4 inverseBind;
};

class Skeleton
{
public:
	std::vector<Joint> joints;

	void UpdateGlobalPose();
	void SetBindPose();
};
