#include "mouseclass.h"


void MouseClass::Initialize()
{


}


void MouseClass::Update()
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(m_Window->hWnd, &p); // ”правление мышью.

    pos = { (float)p.x, (float)p.y, 0.f };
}


point3d MouseClass::GetMouseRay(point3d pos)
{
    XMMATRIX view = m_Camera->GetViewMatrix();
    XMMATRIX proj = m_Camera->GetProjectionMatrix();
    //XMMATRIX proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::state.fovAngle), iaspect, 0.01f, 10000.0f);

    float x = (2.0f * pos.x) / m_Window->width - 1.0f;
    float y = 1.0f - (2.0f * pos.y) / m_Window->height;

    XMVECTOR rayClip = XMVectorSet(x, y, 1.0f, 1.0f);

    XMVECTOR rayEye = XMVector4Transform(rayClip, XMMatrixInverse(nullptr, proj));
    rayEye = XMVectorSet(XMVectorGetX(rayEye), XMVectorGetY(rayEye), 1.0f, 0.0f);

    XMVECTOR ray = XMVector4Transform(rayEye, XMMatrixInverse(nullptr, view));
    point3d rayWorld = point3d(XMVectorGetX(ray), XMVectorGetY(ray), XMVectorGetZ(ray));
    rayWorld = rayWorld.normalized();

    return rayWorld;
}