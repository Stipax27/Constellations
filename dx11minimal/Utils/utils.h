#ifndef _UTILS_H
#define _UTILS_H


//////////////
// INCLUDES //
//////////////
#include <corecrt_math.h>
#include "../Engine/dx11.h"
#include "constants.h"
#include "../Point3d.h"

#include "../entity.h"
#include "../Engine/BasicComponents/Transform.h"
#include "../Engine/BasicComponents/Transform2D.h"



float clamp(float x, float a, float b);
float smoothstep(float edge0, float edge1, float x);
float fract(float);
float lerp(float x1, float x2, float a);

float degreesToRadians(float);
vector<point3d> smoothCornersPath(const vector<point3d>& points, int numberIterations);

DirectX::XMMATRIX GetMatrixFromLookVector(Transform& transform, point3d direction);
DirectX::XMMATRIX GetMatrixFromDirection(point3d direction, point3d upVector = point3d(0.0f, 1.0f, 0.0f));
DirectX::XMMATRIX GetMatrixLookAt(DirectX::XMMATRIX originMatrix, point3d direction);
DirectX::XMMATRIX TransformMatrixToUpVector(DirectX::XMMATRIX sourceMatrix, point3d targetUpVector);

point3d quaternionToEuler(float x, float y, float z, float w);
DirectX::XMVECTOR eulerToQuanternion(float x, float y, float z);


int getRandom(int min = 1, int max = 0);

point3d rotatePoint(point3d, float);

Transform GetWorldTransform(Entity*);
Transform2D GetWorldTransform2D(Entity*);
bool IsEntityValid(Entity*);

vector<string> split(string, string);

float hueToRgb(float, float, float);
point3d hslToRgb(const point3d&);
point3d rgbToHsl(const point3d&);

point3d rotateInPlane(const point3d&, const point3d&, float);

DirectX::XMMATRIX LerpMatrix(const DirectX::XMMATRIX& from, const DirectX::XMMATRIX& to, float t);

#endif