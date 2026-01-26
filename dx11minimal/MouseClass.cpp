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


void MouseClass::Initialize(WindowClass* Window, CameraClass* Camera, CollisionManagerClass* CollisionManager) {
	window = Window;
	camera = Camera;
	collisionManager = CollisionManager;

	state = MouseState::Centered;
	visible = true;
}


void MouseClass::Shutdown()
{
	if (window) {
		window = 0;
	}

	if (camera) {
		camera = 0;
	}

	if (collisionManager) {
		collisionManager = 0;
	}
}


void MouseClass::Update() {
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(window->hWnd, &p);

	absolutePos = { (float)p.x, (float)p.y, 0.f };
	pos = { absolutePos.x / window->width * 2 - 1, -(absolutePos.y / window->height * 2 - 1), 0.0f };
}


point3d MouseClass::GetMouseRay() {
	XMMATRIX view = camera->GetViewMatrix();
	XMMATRIX proj = camera->GetProjectionMatrix();
	//XMMATRIX proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::state.fovAngle), iaspect, 0.01f, 10000.0f);

	float x = (2.0f * absolutePos.x) / window->width - 1.0f;
	float y = 1.0f - (2.0f * absolutePos.y) / window->height;

	XMVECTOR rayClip = XMVectorSet(x, y, 1.0f, 1.0f);

	XMVECTOR rayEye = XMVector4Transform(rayClip, XMMatrixInverse(nullptr, proj));
	rayEye = XMVectorSet(XMVectorGetX(rayEye), XMVectorGetY(rayEye), 1.0f, 0.0f);

	XMVECTOR ray = XMVector4Transform(rayEye, XMMatrixInverse(nullptr, view));
	point3d rayWorld = point3d(XMVectorGetX(ray), XMVectorGetY(ray), XMVectorGetZ(ray));
	rayWorld = rayWorld.normalized();

	RayInfo rayInfo = RayInfo(camera->GetPosition(), rayWorld * 1000.0f);
	RaycastResult result = collisionManager->Raycast(rayInfo);
	if (result.hit) {

	}

	return rayWorld;
}


void MouseClass::RenderCursor() {
	if (visible) {
		ConstBuf::global[0] = XMFLOAT4(pos.x, pos.y, 0.0f, 1.0f);
		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);

		int shaderID = state == MouseState::Free ? 6 : 11;
		Shaders::vShader(shaderID);
		Shaders::pShader(shaderID);
		Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

		context->Draw(6, 0);

		Shaders::vShader(12);
		Shaders::pShader(12);
		Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

		int i = 0;
		DWORD curTime = timer::GetCounter();
		while (i < particles.size())
		{
			MouseParticle particle = particles[i];

			if (curTime - particle.startTime < particle.lifetime)
			{
				if (i < constCount) {
					ConstBuf::global[i] = XMFLOAT4(particle.pos.x, particle.pos.y, particle.angle, 1 - (float)(curTime - particle.startTime) / (float)particle.lifetime);

					//particle.pos += particle.vel * 0.01f;
					//particle.vel *= 0.92f;
				}

				i++;
			}
			else
			{
				particles.erase(particles.begin() + i);
			}
		}

		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);

		size_t size = particles.size();
		if (size > 0) {
			context->DrawInstanced(6, min(size, constCount), 0, 0);
		}
	}
}