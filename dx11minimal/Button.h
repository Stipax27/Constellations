#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "component.h"
#include "point3d.h"

struct Button : Component
{
	bool isClicked = false;
};

#endif