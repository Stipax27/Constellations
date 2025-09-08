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

#include "MouseClass.cpp"

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
};

class LevelManagerClass
{
public:
	WindowStruct* window;

public:
	LevelManagerClass();
	LevelManagerClass(const LevelManagerClass&);
	~LevelManagerClass();

	bool Initialize();
	void Shutdown();
	bool Frame();

private:
	World* m_World;
	MouseClass* m_Mouse;
};

#endif