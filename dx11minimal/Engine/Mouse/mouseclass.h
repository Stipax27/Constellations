#ifndef _MOUSECLASS_H_
#define _MOUSECLASS_H_


//////////////
// INCLUDES //
//////////////
#include "../Types/Point3d.h"
#include "../Camera/cameraclass.h"
#include "../Window/windowclass.h"
#include "../Lib/singleton.h"
#include "../Lib/input.h"

#include <vector>


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
	double startTime;
	DWORD lifetime;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: MouseClass
////////////////////////////////////////////////////////////////////////////////

class MouseClass : public ISingleton
{
public:
	point3d pos;
	point3d absolutePos;
	point3d worldPos;
	MouseState state;
	bool visible;

	std::vector<MouseParticle> particles;

public:
	MouseClass();
	MouseClass(const MouseClass&);
	~MouseClass();

	void Initialize();
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

	point3d GetMouseDirection();
	void RenderCursor();
	void UpdateSystemCursorVisibility();

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

	bool systemCursorHidden;
};

#endif