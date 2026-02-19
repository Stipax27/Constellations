#pragma once
#include <vector>
#include <cstdint>
#include <DirectXMath.h>

#include "../../component.h"

struct SkinnedVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	std::uint32_t joints[4];
	float weights[4];
};

struct SkinnedMesh : Component
{
	std::vector<SkinnedVertex> vertices;
	// Use unsigned long to match Models::CreateModel index buffer type.
	std::vector<unsigned long> indices;

	// Index of the model in the global Models pool (created in UploadToGPU).
	int gpuModelIndex = -1;

	void UploadToGPU();
};