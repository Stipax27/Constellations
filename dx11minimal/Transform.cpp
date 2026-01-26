#include "Transform.h"


point3d Transform::GetRightVector()
{
    return point3d(mRotation.r[0].m128_f32[0], mRotation.r[0].m128_f32[1], mRotation.r[0].m128_f32[2]).normalized();
}

point3d Transform::GetUpVector()
{
    return point3d(mRotation.r[1].m128_f32[0], mRotation.r[1].m128_f32[1], mRotation.r[1].m128_f32[2]).normalized();
}

point3d Transform::GetLookVector()
{
    return point3d(mRotation.r[2].m128_f32[0], mRotation.r[2].m128_f32[1], mRotation.r[2].m128_f32[2]).normalized();
}


Transform& Transform::operator=(const Transform& other) {
    position = other.position;
    scale = other.scale;
    mRotation = other.mRotation;

    return *this;
}

Transform Transform::operator+(const Transform& other) {
    Transform tranform = Transform();

    tranform.position = position + (GetRightVector() * other.position.x + GetUpVector() * other.position.y + GetLookVector() * other.position.z) * scale;
    tranform.scale = scale * other.scale;
    tranform.mRotation = other.mRotation * mRotation;

    return tranform;
}

Transform& Transform::operator+=(const Transform& other) {
    position += (GetRightVector() * other.position.x + GetUpVector() * other.position.y + GetLookVector() * other.position.z) * scale;
    scale *= other.scale;
    mRotation = other.mRotation * mRotation;

    return *this;
}