#ifndef _TRANSFORM_C_
#define _TRANSFORM_C_

#include "component.h"
#include "Point3d.h"
#include "Point3x3.h"


struct Transform : Component
{
	point3d position = point3d();
	point3d rotation = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);

    XMVECTOR qRotation = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

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

void navigationByMouse()
{
    static XMVECTOR currentRotation = XMQuaternionIdentity();

    if (GetAsyncKeyState(VK_RBUTTON))
    {
        float dx = (mouse.pos.x - mouse.oldPos.x) * 0.01;
        float dy = (mouse.pos.y - mouse.oldPos.y) * 0.01;

        XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), dy);
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dx);

        XMVECTOR qNewRotation = XMQuaternionMultiply(qYaw, qPitch);
        Camera::state.currentRotation = XMQuaternionMultiply(qNewRotation, Camera::state.currentRotation);
        Camera::state.currentRotation = XMQuaternionNormalize(Camera::state.currentRotation);

        XMVECTOR rotatedForward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Camera::state.currentRotation);
        XMVECTOR rotatedUp = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), Camera::state.currentRotation);
        XMVECTOR eye = XMVectorScale(rotatedForward, -Camera::state.camDist);

        ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(eye, XMVectorZero(), rotatedUp));

        ConstBuf::UpdateCamera();
        mouse.Angle.x = mouse.oldAngle.x + dx;
        mouse.Angle.y = mouse.oldAngle.y + dy;
        mouse.oldPos.x = mouse.pos.x;
        mouse.oldPos.y = mouse.pos.y;
    }
}

#endif