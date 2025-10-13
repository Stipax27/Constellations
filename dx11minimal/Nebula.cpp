#ifndef _NEBULA_C_
#define _NEBULA_C_

#include "component.h"


struct Nebula : Component
{
	int vShader = 0;
	int pShader = 0;

	int points = 1;
};

#endif