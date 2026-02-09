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

	void Initialize(WindowClass*, CameraClass*);
	void Shutdown();
	void Update();

	bool IsLButtonDown();
	bool IsRButtonDown();
	
	bool IsLButtonClicked();
	bool IsRButtonClicked();

	bool IsLButtonUnclicked();
	bool IsRButtonUnclicked();

	point3d GetLButtonDownPosition();
	point3d GetRButtonDownPosition();

	point3d GetMouseRay();
	void RenderCursor();

private:
	WindowClass* window;
	CameraClass* camera;

	bool lbuttonDown;
	bool rbuttonDown;

	bool lbuttonClicked;
	bool rbuttonClicked;

	bool lbuttonUnclicked;
	bool rbuttonUnclicked;

	point3d lbuttonDownPos;
	point3d rbuttonDownPos;
};

#endif