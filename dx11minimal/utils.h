#pragma once

#include <corecrt_math.h>

#define PI 3.1415926535897932384626433832795f

float clamp(float x, float a, float b);
float smoothstep(float edge0, float edge1, float x);
float fract(float a);
float lerp(float x1, float x2, float a);