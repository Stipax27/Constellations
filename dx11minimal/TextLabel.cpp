#ifndef _TEXTLABEL_C_
#define _TEXTLABEL_C_

#include "component.h"
#include "point3d.h"

struct TextLabel : Component
{
	string text = "TextLabel";
	float fontSize = 8.0f;
};

#endif