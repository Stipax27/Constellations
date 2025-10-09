#ifndef _RENDEROBJECT_C_
#define _RENDEROBJECT_C_

#include "component.h"


struct RenderObject : Component
{
	int vShader = 0;
	int pShader = 0;
};

#endif