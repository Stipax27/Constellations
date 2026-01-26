#ifndef _MOUSECLASS_H_
#define _MOUSECLASS_H_


//////////////
// INCLUDES //
//////////////
#include "Point3d.h"
#include "cameraclass.h"
#include "windowclass.h"
#include <vector>

#include "CollisionManagerClass.h"

using namespace std;

///////////
// ENUMS //
///////////
enum MouseState {
	Free,
	Centered
};

/////////////
// STRUCTS //
/////////////
struct MouseParticle {
	point3d pos;
	float angle;
	DWORD startTime;
	DWORD lifetime;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: MouseClass
////////////////////////////////////////////////////////////////////////////////

class MouseClass
{
public:
	point3d pos;
	point3d absolutePos;
	point3d worldPos;
	MouseState state;
	bool visible;

	vector<MouseParticle> particles;

public:
	MouseClass();
	MouseClass(const MouseClass&);
	~MouseClass();

	void Initialize(WindowClass*, CameraClass*, CollisionManagerClass*);
	void Shutdown();
	void Update();

	point3d GetMouseRay();
	void RenderCursor();

private:
	WindowClass* window;
	CameraClass* camera;
	CollisionManagerClass* collisionManager;
};

#endif