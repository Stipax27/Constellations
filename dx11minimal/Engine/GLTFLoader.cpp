#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "GLTFLoader.h"

#include <tiny_gltf.h>
#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
#include <utility>

using namespace DirectX;

bool GLTFLoader::Load(
	const std::string& path,
	SkinnedMesh& mesh,
	Skeleton& skeleton,
	std::vector<AnimationClip>& animations
)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	bool isGLB = false;
	if (path.size() >= 4)
	{
		isGLB = path.substr(path.size() - 4, 4) == ".glb";
	}

	bool ok = false;
	if (isGLB)
	{
		ok = loader.LoadBinaryFromFile(&model, &err, &warn, path);
	}
	else
	{
		ok = loader.LoadASCIIFromFile(&model, &err, &warn, path);
	}

	if (!ok)
	{
		return false;
	}

	skeleton.joints.clear();

	// Build a simple joint array from nodes (parent links).
	std::vector<int> parentOf(model.nodes.size(), -1);
	for (size_t i = 0; i < model.nodes.size(); ++i)
	{
		for (int child : model.nodes[i].children)
		{
			if (child >= 0 && static_cast<size_t>(child) < parentOf.size())
			{
				parentOf[child] = static_cast<int>(i);
			}
		}
	}

	for (size_t i = 0; i < model.nodes.size(); ++i)
	{
		Joint j;
		j.name = model.nodes[i].name;
		if (j.name.empty())
		{
			j.name = "joint_" + std::to_string(i);
		}
		j.parent = parentOf[i];
		j.children = model.nodes[i].children;

		// Load local transform from node
		const auto& node = model.nodes[i];

		XMMATRIX nodeMat = XMMatrixIdentity();

		// If node has a matrix attribute, use it
		if (node.matrix.size() == 16)
		{
			XMFLOAT4X4 m;
			// glTF stores MAT4 in column-major layout.
			// Engine math uses row-vector convention, so transpose on import.
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					m.m[r][c] = static_cast<float>(node.matrix[r * 4 + c]);
				}
			}
			nodeMat = XMLoadFloat4x4(&m);
		}
		else
		{
			// Otherwise build from translation, rotation, scale
			XMVECTOR translation = XMVectorZero();
			if (node.translation.size() == 3)
			{
				translation = XMVectorSet(
					static_cast<float>(node.translation[0]),
					static_cast<float>(node.translation[1]),
					static_cast<float>(node.translation[2]),
					0.0f
				);
			}

			XMVECTOR rotation = XMQuaternionIdentity();
			if (node.rotation.size() == 4)
			{
				XMVECTOR q = XMVectorSet(
					static_cast<float>(node.rotation[0]),
					static_cast<float>(node.rotation[1]),
					static_cast<float>(node.rotation[2]),
					static_cast<float>(node.rotation[3])
				);
				// Normalize quaternion
				rotation = XMQuaternionNormalize(q);
			}

			XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			if (node.scale.size() == 3)
			{
				scale = XMVectorSet(
					static_cast<float>(node.scale[0]),
					static_cast<float>(node.scale[1]),
					static_cast<float>(node.scale[2]),
					1.0f
				);
			}

			XMMATRIX T = XMMatrixTranslationFromVector(translation);
			XMMATRIX R = XMMatrixRotationQuaternion(rotation);
			XMMATRIX S = XMMatrixScalingFromVector(scale);

			// Engine transform convention: S * R * T
			nodeMat = S * R * T;
		}

		XMStoreFloat4x4(&j.local, nodeMat);
		j.global = j.local;
		j.inverseBind = j.local;
		skeleton.joints.push_back(j);
	}

	// Initialize global poses from local transforms
	skeleton.UpdateGlobalPose();

	// Resolve mesh node and the skin used by that node.
	int meshIndex = 0; // we load first mesh primitive
	int meshNodeIndex = -1;
	int skinIndex = -1;
	for (size_t ni = 0; ni < model.nodes.size(); ++ni)
	{
		if (model.nodes[ni].mesh == meshIndex)
		{
			if (meshNodeIndex < 0)
			{
				meshNodeIndex = static_cast<int>(ni);
				skinIndex = model.nodes[ni].skin;
			}
		}
	}

	const tinygltf::Skin* skinPtr = nullptr;
	if (skinIndex >= 0 && static_cast<size_t>(skinIndex) < model.skins.size())
	{
		skinPtr = &model.skins[skinIndex];
	}
	else if (!model.skins.empty())
	{
		skinPtr = &model.skins[0];
		skinIndex = 0;
	}

	// Fallback inverse bind from computed bind pose.
	// Row-vector convention:
	// inverseBind = meshGlobal * inverse(jointGlobalBind)
	XMMATRIX meshGlobal = XMMatrixIdentity();
	if (meshNodeIndex >= 0 && static_cast<size_t>(meshNodeIndex) < skeleton.joints.size())
	{
		meshGlobal = XMLoadFloat4x4(&skeleton.joints[meshNodeIndex].global);
	}
	for (size_t i = 0; i < skeleton.joints.size(); ++i)
	{
		const XMMATRIX jointGlobal = XMLoadFloat4x4(&skeleton.joints[i].global);
		const XMMATRIX invJoint = XMMatrixInverse(nullptr, jointGlobal);
		const XMMATRIX invBind = meshGlobal * invJoint;
		XMStoreFloat4x4(&skeleton.joints[i].inverseBind, invBind);
	}

	// Prefer inverse bind matrices from glTF skin when present.
	if (skinPtr &&
		skinPtr->inverseBindMatrices >= 0 &&
		static_cast<size_t>(skinPtr->inverseBindMatrices) < model.accessors.size())
	{
		const auto& acc = model.accessors[skinPtr->inverseBindMatrices];
		if (acc.bufferView >= 0 &&
			static_cast<size_t>(acc.bufferView) < model.bufferViews.size())
		{
			const auto& bv = model.bufferViews[acc.bufferView];
			if (static_cast<size_t>(bv.buffer) < model.buffers.size())
			{
				const auto& b = model.buffers[bv.buffer];
				const unsigned char* base = b.data.data() + bv.byteOffset + acc.byteOffset;
				int stride = acc.ByteStride(bv);
				if (stride <= 0)
				{
					stride = sizeof(float) * 16; // MAT4 float
				}

				const size_t n = std::min<size_t>(
					static_cast<size_t>(acc.count), skinPtr->joints.size());

				for (size_t i = 0; i < n; ++i)
				{
					const int nodeIndex = skinPtr->joints[i];
					if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= skeleton.joints.size())
					{
						continue;
					}

					auto m = reinterpret_cast<const float*>(
						base + i * static_cast<size_t>(stride));

					XMFLOAT4X4 ib{};
					// glTF matrices are stored in column-major layout.
					// Engine math uses row-vector convention, so transpose on import.
					for (int r = 0; r < 4; ++r)
					{
						for (int c = 0; c < 4; ++c)
						{
							ib.m[r][c] = m[r * 4 + c];
						}
					}

					skeleton.joints[nodeIndex].inverseBind = ib;
				}
			}
		}
	}

	animations.clear();
	auto readAccessorData = [&](int accessorIndex, const unsigned char*& base, int& stride) -> bool
	{
		if (accessorIndex < 0 || static_cast<size_t>(accessorIndex) >= model.accessors.size())
		{
			return false;
		}

		const auto& acc = model.accessors[accessorIndex];
		if (acc.bufferView < 0 || static_cast<size_t>(acc.bufferView) >= model.bufferViews.size())
		{
			return false;
		}

		const auto& bv = model.bufferViews[acc.bufferView];
		if (bv.buffer < 0 || static_cast<size_t>(bv.buffer) >= model.buffers.size())
		{
			return false;
		}

		base = model.buffers[bv.buffer].data.data() + bv.byteOffset + acc.byteOffset;
		stride = acc.ByteStride(bv);
		return true;
	};

	for (size_t animIdx = 0; animIdx < model.animations.size(); ++animIdx)
	{
		const auto& srcAnim = model.animations[animIdx];
		AnimationClip clip;
		clip.name = srcAnim.name.empty() ? ("Animation_" + std::to_string(animIdx)) : srcAnim.name;

		for (const auto& ch : srcAnim.channels)
		{
			if (ch.sampler < 0 || static_cast<size_t>(ch.sampler) >= srcAnim.samplers.size())
			{
				continue;
			}

			const auto& sampler = srcAnim.samplers[ch.sampler];
			AnimationChannel channel;
			channel.joint = ch.target_node;
			channel.path = ch.target_path;

			const unsigned char* inputBase = nullptr;
			int inputStride = 0;
			if (!readAccessorData(sampler.input, inputBase, inputStride))
			{
				continue;
			}

			const auto& inputAcc = model.accessors[sampler.input];
			if (inputStride <= 0)
			{
				inputStride = sizeof(float);
			}

			channel.times.resize(inputAcc.count);
			for (size_t i = 0; i < inputAcc.count; ++i)
			{
				auto tPtr = reinterpret_cast<const float*>(inputBase + i * static_cast<size_t>(inputStride));
				channel.times[i] = tPtr[0];
				if (channel.times[i] > clip.duration)
				{
					clip.duration = channel.times[i];
				}
			}

			const unsigned char* outputBase = nullptr;
			int outputStride = 0;
			if (!readAccessorData(sampler.output, outputBase, outputStride))
			{
				continue;
			}

			const auto& outputAcc = model.accessors[sampler.output];
			const bool isRotation = (channel.path == "rotation");
			const int compCount = isRotation ? 4 : 3;
			if (outputStride <= 0)
			{
				outputStride = sizeof(float) * compCount;
			}

			const bool isCubicSpline = (sampler.interpolation == "CUBICSPLINE");
			const size_t keyCount = channel.times.size();
			channel.values.resize(keyCount);

			for (size_t i = 0; i < keyCount; ++i)
			{
				size_t srcIndex = isCubicSpline ? (i * 3 + 1) : i;
				if (srcIndex >= outputAcc.count)
				{
					srcIndex = outputAcc.count > 0 ? (outputAcc.count - 1) : 0;
				}

				auto vPtr = reinterpret_cast<const float*>(outputBase + srcIndex * static_cast<size_t>(outputStride));
				if (isRotation)
				{
					XMStoreFloat4(&channel.values[i], XMVectorSet(vPtr[0], vPtr[1], vPtr[2], vPtr[3]));
				}
				else
				{
					XMStoreFloat4(&channel.values[i], XMVectorSet(vPtr[0], vPtr[1], vPtr[2], 0.0f));
				}
			}

			clip.channels.push_back(std::move(channel));
		}

		animations.push_back(std::move(clip));
	}

	// Load first mesh primitive as skinned mesh geometry.
	if (!model.meshes.empty() && !model.meshes[0].primitives.empty())
	{
		const auto& prim = model.meshes[0].primitives[0];
		auto readPrimitiveAccessorData = [&](int accessorIndex, const tinygltf::Accessor*& acc, const unsigned char*& base, int& stride) -> bool
		{
			if (accessorIndex < 0 || static_cast<size_t>(accessorIndex) >= model.accessors.size())
			{
				return false;
			}

			acc = &model.accessors[accessorIndex];
			if (acc->bufferView < 0 || static_cast<size_t>(acc->bufferView) >= model.bufferViews.size())
			{
				return false;
			}

			const auto& bv = model.bufferViews[acc->bufferView];
			if (bv.buffer < 0 || static_cast<size_t>(bv.buffer) >= model.buffers.size())
			{
				return false;
			}

			base = model.buffers[bv.buffer].data.data() + bv.byteOffset + acc->byteOffset;
			stride = acc->ByteStride(bv);
			return true;
		};

		// Prepare skin joint mapping: JOINTS_0 values are indices into skin.joints.
		// We'll map those to actual node indices so vertex joint indices match skeleton.joints
		// which was created by iterating model.nodes.
		std::vector<int> skinJointNodeIndex;
		if (skinPtr)
		{
			skinJointNodeIndex = skinPtr->joints;
		}

		// Resize vertex array based on POSITION accessor (we'll fill other attributes below).
		auto posIt = prim.attributes.find("POSITION");
		if (posIt != prim.attributes.end())
		{
			if (posIt->second < 0 || static_cast<size_t>(posIt->second) >= model.accessors.size())
			{
				return true;
			}

			const auto& acc = model.accessors[posIt->second];
			mesh.vertices.resize(acc.count);

			// Initialize all weights and joints to default values
			for (size_t i = 0; i < acc.count; ++i)
			{
				mesh.vertices[i].joints[0] = 0;
				mesh.vertices[i].joints[1] = 0;
				mesh.vertices[i].joints[2] = 0;
				mesh.vertices[i].joints[3] = 0;
				mesh.vertices[i].weights[0] = 1.0f; // First weight = 1.0
				mesh.vertices[i].weights[1] = 0.0f;
				mesh.vertices[i].weights[2] = 0.0f;
				mesh.vertices[i].weights[3] = 0.0f;
			}

			if (acc.bufferView >= 0 &&
				static_cast<size_t>(acc.bufferView) < model.bufferViews.size())
			{
				const auto& bv = model.bufferViews[acc.bufferView];
				if (static_cast<size_t>(bv.buffer) < model.buffers.size())
				{
					const auto& b = model.buffers[bv.buffer];

					const unsigned char* base =
						b.data.data() + bv.byteOffset + acc.byteOffset;
					int stride = acc.ByteStride(bv);
					if (stride <= 0)
					{
						stride = sizeof(float) * 3; // vec3
					}

					for (size_t i = 0; i < acc.count; ++i)
					{
						auto v = reinterpret_cast<const float*>(base + i * static_cast<size_t>(stride));
						mesh.vertices[i].position.x = v[0];
						mesh.vertices[i].position.y = v[1];
						mesh.vertices[i].position.z = v[2];
					}
				}
			}
		}

		// Normals (optional)
		auto nrmIt = prim.attributes.find("NORMAL");
		if (nrmIt != prim.attributes.end() &&
			nrmIt->second >= 0 &&
			static_cast<size_t>(nrmIt->second) < model.accessors.size())
		{
			const tinygltf::Accessor* acc = nullptr;
			const unsigned char* base = nullptr;
			int stride = 0;
			if (readPrimitiveAccessorData(nrmIt->second, acc, base, stride))
			{
				if (stride <= 0)
				{
					stride = sizeof(float) * 3; // vec3
				}

				for (size_t i = 0; i < acc->count && i < mesh.vertices.size(); ++i)
				{
					auto v = reinterpret_cast<const float*>(base + i * static_cast<size_t>(stride));
					mesh.vertices[i].normal.x = v[0];
					mesh.vertices[i].normal.y = v[1];
					mesh.vertices[i].normal.z = v[2];
				}
			}
		}

		// UVs (optional)
		auto uvIt = prim.attributes.find("TEXCOORD_0");
		if (uvIt != prim.attributes.end() &&
			uvIt->second >= 0 &&
			static_cast<size_t>(uvIt->second) < model.accessors.size())
		{
			const tinygltf::Accessor* acc = nullptr;
			const unsigned char* base = nullptr;
			int stride = 0;
			if (readPrimitiveAccessorData(uvIt->second, acc, base, stride))
			{
				if (stride <= 0)
				{
					stride = sizeof(float) * 2; // vec2
				}

				for (size_t i = 0; i < acc->count && i < mesh.vertices.size(); ++i)
				{
					auto v = reinterpret_cast<const float*>(base + i * static_cast<size_t>(stride));
					mesh.vertices[i].uv.x = v[0];
					mesh.vertices[i].uv.y = v[1];
				}
			}
		}

		// Joint indices (optional, VEC4 of unsigned bytes or ushorts)
		auto jointsIt = prim.attributes.find("JOINTS_0");
		if (jointsIt != prim.attributes.end() &&
			jointsIt->second >= 0 &&
			static_cast<size_t>(jointsIt->second) < model.accessors.size())
		{
			const tinygltf::Accessor* acc = nullptr;
			const unsigned char* base = nullptr;
			int stride = 0;
			if (readPrimitiveAccessorData(jointsIt->second, acc, base, stride))
			{
				if (stride <= 0)
				{
					stride = 4; // 4 x u8 fallback
				}

				for (size_t i = 0; i < acc->count && i < mesh.vertices.size(); ++i)
				{
					const unsigned char* src = base + i * static_cast<size_t>(stride);

					if (acc->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						for (int k = 0; k < 4; ++k)
						{
							uint32_t raw = src[k];
							if (!skinJointNodeIndex.empty() && raw < skinJointNodeIndex.size())
							{
								mesh.vertices[i].joints[k] = static_cast<uint32_t>(skinJointNodeIndex[raw]);
							}
							else
							{
								mesh.vertices[i].joints[k] = raw;
							}
						}
					}
					else if (acc->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						auto s = reinterpret_cast<const uint16_t*>(src);
						for (int k = 0; k < 4; ++k)
						{
							uint32_t raw = s[k];
							if (!skinJointNodeIndex.empty() && raw < skinJointNodeIndex.size())
							{
								mesh.vertices[i].joints[k] = static_cast<uint32_t>(skinJointNodeIndex[raw]);
							}
							else
							{
								mesh.vertices[i].joints[k] = raw;
							}
						}
					}
				}
			}
		}

		// Weights (optional, VEC4 of floats)
		auto wgtIt = prim.attributes.find("WEIGHTS_0");
		if (wgtIt != prim.attributes.end() &&
			wgtIt->second >= 0 &&
			static_cast<size_t>(wgtIt->second) < model.accessors.size())
		{
			const tinygltf::Accessor* acc = nullptr;
			const unsigned char* base = nullptr;
			int stride = 0;
			if (readPrimitiveAccessorData(wgtIt->second, acc, base, stride))
			{
				if (stride <= 0)
				{
					stride = sizeof(float) * 4; // vec4
				}

				for (size_t i = 0; i < acc->count && i < mesh.vertices.size(); ++i)
				{
					auto v = reinterpret_cast<const float*>(base + i * static_cast<size_t>(stride));
					for (int k = 0; k < 4; ++k)
						mesh.vertices[i].weights[k] = v[k];
				}
			}
		}

		// Normalize weights to guard against malformed exporters that don't ensure
		// weights sum to 1.0. This keeps skinning stable even if sums slightly differ.
		for (size_t vi = 0; vi < mesh.vertices.size(); ++vi)
		{
			float s = mesh.vertices[vi].weights[0] + mesh.vertices[vi].weights[1]
				+ mesh.vertices[vi].weights[2] + mesh.vertices[vi].weights[3];
			if (s > 1e-6f && fabsf(s - 1.0f) > 1e-4f)
			{
				mesh.vertices[vi].weights[0] /= s;
				mesh.vertices[vi].weights[1] /= s;
				mesh.vertices[vi].weights[2] /= s;
				mesh.vertices[vi].weights[3] /= s;
			}
		}

		// Keep vertex positions in original glTF bind-pose space.
		// Re-centering/re-scaling vertices here breaks skinning unless the same
		// transform is applied to skeleton bind data and animation.

		// Indices
		if (prim.indices >= 0 &&
			static_cast<size_t>(prim.indices) < model.accessors.size())
		{
			const auto& acc = model.accessors[prim.indices];
			if (acc.bufferView >= 0 &&
				static_cast<size_t>(acc.bufferView) < model.bufferViews.size())
			{
				const auto& bv = model.bufferViews[acc.bufferView];
				if (static_cast<size_t>(bv.buffer) < model.buffers.size())
				{
					const auto& b = model.buffers[bv.buffer];
					mesh.indices.resize(acc.count);

					const unsigned char* base =
						b.data.data() + bv.byteOffset + acc.byteOffset;

					int componentSize = 0;
					if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						componentSize = sizeof(uint16_t);
					}
					else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						componentSize = sizeof(uint32_t);
					}
					else
					{
						componentSize = sizeof(uint16_t); // fallback
					}

					int stride = acc.ByteStride(bv);
					if (stride <= 0)
					{
						stride = componentSize;
					}

					for (size_t i = 0; i < acc.count; ++i)
					{
						const unsigned char* ptr = base + i * static_cast<size_t>(stride);
						if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						{
							mesh.indices[i] = static_cast<unsigned long>(
								*reinterpret_cast<const uint16_t*>(ptr));
						}
						else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
						{
							mesh.indices[i] = static_cast<unsigned long>(
								*reinterpret_cast<const uint32_t*>(ptr));
						}
						else
						{
							mesh.indices[i] = static_cast<unsigned long>(
								*reinterpret_cast<const uint16_t*>(ptr));
						}
					}
				}
			}
		}
		else if (!mesh.vertices.empty())
		{
			// glTF primitive without indices: create 0..N-1 index buffer
			// so UploadToGPU can still upload and draw this mesh.
			mesh.indices.resize(mesh.vertices.size());
			for (size_t i = 0; i < mesh.vertices.size(); ++i)
			{
				mesh.indices[i] = static_cast<unsigned long>(i);
			}
		}
	}

	return true;
}

bool GLTFLoader::Load(
	const std::string& path,
	SkinnedMesh& mesh,
	Skeleton& skeleton,
	AnimationClip& animation
)
{
	std::vector<AnimationClip> clips;
	if (!Load(path, mesh, skeleton, clips))
	{
		return false;
	}

	if (!clips.empty())
	{
		animation = clips[0];
	}
	else
	{
		animation = AnimationClip{};
	}

	return true;
}
