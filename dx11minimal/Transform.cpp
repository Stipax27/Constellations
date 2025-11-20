#ifndef _TRANSFORM_C_
#define _TRANSFORM_C_

#include "component.h"
#include "Point3d.h"
#include <DirectXMath.h>


struct Transform : Component
{
	point3d position = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX mRotation = DirectX::XMMatrixIdentity();


    point3d GetRightVector()
    {
        return point3d(mRotation.r[0].m128_f32[0], mRotation.r[0].m128_f32[1], mRotation.r[0].m128_f32[2]).normalized();
    }

    point3d GetUpVector()
    {
        return point3d(mRotation.r[1].m128_f32[0], mRotation.r[1].m128_f32[1], mRotation.r[1].m128_f32[2]).normalized();
    }

    point3d GetLookVector()
    {
        return point3d(mRotation.r[2].m128_f32[0], mRotation.r[2].m128_f32[1], mRotation.r[2].m128_f32[2]).normalized();
    }


    Transform& operator=(const Transform& other) {
        position = other.position;
        scale = other.scale;
        mRotation = other.mRotation;

        return *this;
    }

    Transform operator+(const Transform& other) {
        Transform tranform = Transform();

        tranform.position = position + (GetRightVector() * other.position.x + GetUpVector() * other.position.y + GetLookVector() * other.position.z) * scale;
        tranform.scale = scale * other.scale;
        tranform.mRotation = other.mRotation * mRotation;

        return tranform;
    }

    Transform& operator+=(const Transform& other) {
        position += (GetRightVector() * other.position.x + GetUpVector() * other.position.y + GetLookVector() * other.position.z) * scale;
        scale *= other.scale;
        mRotation = other.mRotation * mRotation;

        return *this;
    }
};


#endif