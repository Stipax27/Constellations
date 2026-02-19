#include "Animator.h"
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

void Animator::Update(Skeleton& skeleton, const AnimationClip& clip, float time)
{
	// Validate
	if (clip.duration <= 0.0f || clip.channels.empty())
	{
		// No animation - keep skeleton in bind pose
		return;
	}

	// Ensure time is within range
	float t = fmod(time, clip.duration);
	if (t < 0.0f)
	{
		t += clip.duration;
	}

	// Update each joint
	for (size_t jointIdx = 0; jointIdx < skeleton.joints.size(); jointIdx++)
	{
		// Default values - initialize from current local (bind pose) so we don't
		// overwrite non-animated joints with identity.
		XMMATRIX currentLocal = XMLoadFloat4x4(&skeleton.joints[jointIdx].local);
		XMVECTOR translation = XMVectorZero();
		XMVECTOR rotation = XMQuaternionIdentity();
		XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		// Try to decompose existing local matrix. If decomposition fails, fall
		// back to safe defaults above.
		XMVECTOR s, r, tr;
		if (XMMatrixDecompose(&s, &r, &tr, currentLocal))
		{
			scale = s;
			rotation = r;
			translation = tr;
		}

		bool hasAnyChannel = false;

		// Find animation channels for this joint
		for (const auto& channel : clip.channels)
		{
			// Skip if not for this joint
			if (channel.joint != static_cast<int>(jointIdx))
			{
				continue;
			}

			hasAnyChannel = true;

			// Skip if no keyframes
			if (channel.times.empty() || channel.values.empty())
			{
				continue;
			}

			// Find current keyframe bracket
			size_t keyframeIdx = 0;
			for (size_t i = 0; i + 1 < channel.times.size(); ++i)
			{
				if (t < channel.times[i + 1])
				{
					keyframeIdx = i;
					break;
				}
				keyframeIdx = i;
			}

			// Clamp keyframe index
			keyframeIdx = std::min(keyframeIdx, channel.times.size() - 1);

			// Get current and next keyframe
			XMVECTOR keyA = XMLoadFloat4(&channel.values[keyframeIdx]);
			XMVECTOR keyB = keyA;

			// Interpolation factor
			float alpha = 0.0f;
			if (keyframeIdx + 1 < channel.times.size())
			{
				float t0 = channel.times[keyframeIdx];
				float t1 = channel.times[keyframeIdx + 1];
				float dt = t1 - t0;

				if (dt > 0.0001f)
				{
					alpha = std::max(0.0f, std::min(1.0f, (t - t0) / dt));
					keyB = XMLoadFloat4(&channel.values[keyframeIdx + 1]);
				}
			}

			// Apply animation based on channel type
			if (channel.path == "translation")
			{
				translation = XMVectorLerp(keyA, keyB, alpha);
			}
			else if (channel.path == "rotation")
			{
				// Normalize quaternions
				keyA = XMQuaternionNormalize(keyA);
				keyB = XMQuaternionNormalize(keyB);

				// Handle shortest path
				float dot = XMVectorGetX(XMQuaternionDot(keyA, keyB));
				if (dot < 0.0f)
				{
					keyB = XMVectorNegate(keyB);
				}

				// SLERP
				rotation = XMQuaternionSlerp(keyA, keyB, alpha);
				rotation = XMQuaternionNormalize(rotation);
			}
			else if (channel.path == "scale")
			{
				scale = XMVectorLerp(keyA, keyB, alpha);
			}

		}

		// If there were no channels targeting this joint, keep original local matrix.
		if (!hasAnyChannel)
		{
			continue; // leave skeleton.joints[jointIdx].local untouched
		}

		// Build local transform matrix in engine convention: S * R * T.
		XMMATRIX matT = XMMatrixTranslationFromVector(translation);
		XMMATRIX matR = XMMatrixRotationQuaternion(rotation);
		XMMATRIX matS = XMMatrixScalingFromVector(scale);

		XMMATRIX localMatrix = matS * matR * matT;
		XMStoreFloat4x4(&skeleton.joints[jointIdx].local, localMatrix);
	}

	// Update global poses from local transforms
	skeleton.UpdateGlobalPose();
}
