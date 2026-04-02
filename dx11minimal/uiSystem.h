#ifndef _UI_SYSTEM_H_
#define _UI_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform2D.h"
#include "Rect.h"
#include "Button.h"
#include "Components/TextLabel.h"
//#include "font.h"

#include "mouseclass.h"

/////////////
// GLOBALS //
/////////////
const bool SHOW_UI_ANCHOR_POINTS = false;


class UISystem : public System
{
public:
	UISystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);

private:
	MouseClass* mouse;
	EntityStorage* entityStorage;

private:
	void DrawUiObject(Transform2D, int);
};

#endif
