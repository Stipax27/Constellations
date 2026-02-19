#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

struct AnimationChannel
{
	int joint;
	std::string path;
	std::vector<float> times;
	std::vector<DirectX::XMFLOAT4> values;
};

struct AnimationClip
{
	std::string name;
	float duration = 0.0f;
	std::vector<AnimationChannel> channels;
};
