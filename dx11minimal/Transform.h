#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "component.h"
#include "Point3d.h"
#include <DirectXMath.h>


struct Transform : Component
{
    point3d position = point3d();
    point3d scale = point3d(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX mRotation = DirectX::XMMatrixIdentity();

    point3d GetRightVector();
    point3d GetUpVector();
    point3d GetLookVector();

    Transform& operator=(const Transform&);
    Transform operator+(const Transform&);
    Transform& operator+=(const Transform&);
};


#endif