#ifndef _MOUSECLASS_H_
#define _MOUSECLASS_H_


//////////////
// INCLUDES //
//////////////
#include "Point3d.h"
#include "cameraclass.h"
#include "windowclass.h"

///////////
// ENUMS //
///////////
enum MouseState {
	Free,
	Centered
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