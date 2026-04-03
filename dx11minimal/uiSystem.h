#ifndef _UI_SYSTEM_H_
#define _UI_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Engine/BasicComponents/Transform2D.h"
#include "Rect.h"
#include "Button.h"
#include "Components/TextLabel.h"
//#include "font.h"

#include "Engine/Mouse/mouseclass.h"

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

	void Update(EntityStorage&, float);

private:
	MouseClass* mouse;

private:
	void DrawUiObject(Transform2D, int);
};

#endif
