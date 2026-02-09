#ifndef _UTILS_H
#define _UTILS_H


//////////////
// INCLUDES //
//////////////
#include <corecrt_math.h>
#include "dx11.h"
#include "constants.h"
#include "Point3d.h"

#include "entity.h"
#include "Transform.h"
#include "Transform2D.h"



float clamp(float x, float a, float b);
float smoothstep(float edge0, float edge1, float x);
float fract(float);
float lerp(float x1, float x2, float a);

float degreesToRadians(float);
vector<point3d> smoothCornersPath(const vector<point3d>& points, int numberIterations);
void SetLookVector(Transform* transform, point3d direction);
point3d quaternionToEuler(float x, float y, float z, float w);
DirectX::XMVECTOR eulerToQuanternion(float x, float y, float z);

int getRandom(int min = 1, int max = 0);

point3d rotatePoint(point3d, float);

Transform GetWorldTransform(Entity*);
Transform2D GetWorldTransform2D(Entity*);
bool IsEntityValid(Entity*);

bool IsKeyPressed(const int);

vector<string> split(string, string);

float hueToRgb(float, float, float);
point3d hslToRgb(const point3d&);
point3d rgbToHsl(const point3d&);

point3d rotateInPlane(const point3d&, const point3d&, float);

#endif