#ifndef _TRANSFORM_C_
#define _TRANSFORM_C_

#include "component.h"
#include "Point3d.h"


struct Transform : Component
{
	point3d position = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);

    XMVECTOR qRotation = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0);

    XMMATRIX mRotation = XMMATRIX{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    XMVECTOR GetLookVector()
	{
        return XMVector3Rotate(XMVectorSet(0, 0, 1, 0), qRotation);
	}

    XMVECTOR GetUpVector()
    {
        return XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qRotation);
    }

    XMVECTOR GetRightVector()
    {
        return XMVector3Rotate(XMVectorSet(1, 0, 0, 0), qRotation);
    }
};

#endif