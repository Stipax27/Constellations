#include "SkinnedMesh.h"
#include "../../dx11.h"

void SkinnedMesh::UploadToGPU()
{
	if (vertices.empty() || indices.empty())
		return;

	// Allocate a new slot in the shared Models pool.
	int modelIndex = Models::modelsCount;
	if (modelIndex >= max_models)
		return;

	// CreateModel uses global vertex/index counters.
	Models::vertexCount = static_cast<int>(vertices.size());
	Models::indexCount = static_cast<int>(indices.size());

	// Convert skinned vertices to the shared vertex format.
	auto verts = new Models::VertexType[vertices.size()];
	for (size_t i = 0; i < vertices.size(); i++)
	{
		verts[i].position = vertices[i].position;
		verts[i].normal = vertices[i].normal;
		verts[i].texture = vertices[i].uv;

		verts[i].joints = XMUINT4(
			vertices[i].joints[0],
			vertices[i].joints[1],
			vertices[i].joints[2],
			vertices[i].joints[3]);

		verts[i].weights = XMFLOAT4(
			vertices[i].weights[0],
			vertices[i].weights[1],
			vertices[i].weights[2],
			vertices[i].weights[3]);
	}

	Models::CreateModel(modelIndex, verts, indices.data());

	gpuModelIndex = modelIndex;
	Models::Model[modelIndex].indexes = Models::indexCount;
	Models::modelsCount = modelIndex + 1;

	delete[] verts;
}
