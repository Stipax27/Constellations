////////////////////////////////////////////////////////////////////////////////
// Filename: LevelManagerClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LEVELMANAGERCLASS_H_
#define _LEVELMANAGERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"

#include "world.h"
#include "system.h"
#include "entity.h"
#include "component.h"

#include "Transform.cpp"
#include "PhysicBody.cpp"
#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "Sprite.cpp"

#include "PhysicSystem.cpp"
#include "CollisionSystem.cpp"
#include "RenderSystem.cpp"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: LevelManagerClass
////////////////////////////////////////////////////////////////////////////////

struct WindowStruct {
	HWND hWnd; // window handle
	HDC device_context, context; // 2 device contexts (for bufferization)
	int width, height; // saving window sizes the program created
	float aspect, iaspect;

	void Initialize() {
		RECT rect;
		GetClientRect(hWnd, &rect);

		device_context = GetDC(hWnd);

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		aspect = float(height) / float(width);
		iaspect = float(width) / float(height);

		// window params into const buffer
		ConstBuf::frame.aspect = XMFLOAT4{ aspect, iaspect, float(width), float(height) };
	}
};

struct MouseStruct {
public:

	void Initialize(WindowStruct* window, CameraClass* camera) {
		m_Window = window;
		m_Camera = camera;
	}

	void Update() {
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(m_Window->hWnd, &p);

		pos = { (float)p.x, (float)p.y, 0.f };
	}

	point3d GetMouseRay(point3d) {
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

private:
	point3d pos;
	point3d oldPos;
	point3d worldPos;

	WindowStruct* m_Window;
	CameraClass* m_Camera;
};

class LevelManagerClass
{
public:
	WindowStruct* window;
	MouseStruct* mouse;

public:
	LevelManagerClass();
	LevelManagerClass(const LevelManagerClass&);
	~LevelManagerClass();

	void InitWindow();
	bool Initialize();
	void Shutdown();

	bool Frame();

	void ProcessSound(const char* name)//проигрывание аудиофайла в формате .wav, файл должен лежать в той же папке где и программа
	{
		//PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//переменная name содежрит имя файла. флаг ASYNC позволяет проигрывать звук паралельно с исполнением программы
	}

private:
	World* m_World;
};

#endif