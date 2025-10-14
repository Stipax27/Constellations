#include "mouseclass.h"

MouseClass::MouseClass()
{
	window = 0;
	camera = 0;
}

MouseClass::MouseClass(const MouseClass& other)
{
}


MouseClass::~MouseClass()
{
}


void MouseClass::Initialize(WindowClass* Window, CameraClass* Camera) {
	window = Window;
	camera = Camera;
}


void MouseClass::Shutdown()
{
	if (window)
	{
		window = 0;
	}

	if (camera)
	{
		camera = 0;
	}
}


void MouseClass::Update() {
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(window->hWnd, &p);

	pos = { (float)p.x, (float)p.y, 0.f };
}


point3d MouseClass::GetMouseRay() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjectionMatrix();
	//XMMATRIX proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::state.fovAngle), iaspect, 0.01f, 10000.0f);

	float x = (2.0f * pos.x) / window->width - 1.0f;
	float y = 1.0f - (2.0f * pos.y) / window->height;

	XMVECTOR rayClip = XMVectorSet(x, y, 1.0f, 1.0f);

	XMVECTOR rayEye = XMVector4Transform(rayClip, XMMatrixInverse(nullptr, proj));
	rayEye = XMVectorSet(XMVectorGetX(rayEye), XMVectorGetY(rayEye), 1.0f, 0.0f);

	XMVECTOR ray = XMVector4Transform(rayEye, XMMatrixInverse(nullptr, view));
	point3d rayWorld = point3d(XMVectorGetX(ray), XMVectorGetY(ray), XMVectorGetZ(ray));
	rayWorld = rayWorld.normalized();

	return rayWorld;
}