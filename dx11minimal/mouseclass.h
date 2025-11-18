#ifndef _MOUSECLASS_H_
#define _MOUSECLASS_H_


//////////////
// INCLUDES //
//////////////
#include "Point3d.h"
#include "cameraclass.h"
#include "windowclass.h"
#include <vector>

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
	point3d vel;
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
	point3d oldPos;
	point3d worldPos;
	MouseState state;

	vector<MouseParticle> particles;

public:
	MouseClass();
	MouseClass(const MouseClass&);
	~MouseClass();

	void Initialize(WindowClass* window, CameraClass* camera);
	void Shutdown();
	void Update();

	point3d GetMouseRay();
	void RenderCursor();

private:
	WindowClass* window;
	CameraClass* camera;
};

#endif