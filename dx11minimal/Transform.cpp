#ifndef _TRANSFORM_C_
#define _TRANSFORM_C_

#include "component.h"
#include "Point3d.h"
#include <DirectXMath.h>


struct Transform : Component
{
	point3d position = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);

    DirectX::XMVECTOR qRotation = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0);

    DirectX::XMMATRIX mRotation = DirectX::XMMATRIX{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    point3d GetLookVector()
	{
        return point3d(mRotation.r[0].m128_f32[2], mRotation.r[1].m128_f32[2], mRotation.r[2].m128_f32[2]);
	}

    point3d GetUpVector()
    {
        return point3d(mRotation.r[0].m128_f32[1], mRotation.r[1].m128_f32[1], mRotation.r[2].m128_f32[1]);
    }

    point3d GetRightVector()
    {
        return point3d(mRotation.r[0].m128_f32[0], mRotation.r[1].m128_f32[0], mRotation.r[2].m128_f32[0]);
    }
};

#endif