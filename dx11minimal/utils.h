#ifndef _UTILS_H
#define _UTILS_H


//////////////
// INCLUDES //
//////////////
#include <corecrt_math.h>
#include "Point3d.h"


/////////////
// GLOBALS //
/////////////
#define PI 3.1415926535897932384626433832795f
#define RAD 0.017453292519943295f


float clamp(float x, float a, float b);
float smoothstep(float edge0, float edge1, float x);
float fract(float);
float lerp(float x1, float x2, float a);
float degreesToRadians(float);
point3d quaternionToEuler(float x, float y, float z, float w);
XMVECTOR eulerToQuanternion(float x, float y, float z);

#endif