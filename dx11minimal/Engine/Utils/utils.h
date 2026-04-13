#ifndef _UTILS_H
#define _UTILS_H


//////////////
// INCLUDES //
//////////////
#include <corecrt_math.h>
#include <vector>
#include <string>

#include "../dx11.h"
#include "constants.h"
#include "../Types/Point3d.h"

#include "../ECS_Base/entity.h"
#include "../BasicComponents/Transform.h"
#include "../BasicComponents/Transform2D.h"



float clamp(float x, float a, float b);
float smoothstep(float edge0, float edge1, float x);
float fract(float);
//float lerp(float x1, float x2, float a);

float degreesToRadians(float);
std::vector<point3d> smoothCornersPath(const std::vector<point3d>& points, int numberIterations);

DirectX::XMMATRIX GetMatrixFromLookVector(Transform& transform, point3d direction);
DirectX::XMMATRIX GetMatrixFromDirection(const point3d& direction, const point3d& upVector = point3d(0.0f, 1.0f, 0.0f));
DirectX::XMMATRIX GetMatrixLookAt(const DirectX::XMMATRIX& originMatrix, const point3d& direction);
DirectX::XMMATRIX TransformMatrixToUpVector(const DirectX::XMMATRIX& sourceMatrix, const point3d& targetUpVector);

float GetSignedAngleBetweenLookVectors(const XMMATRIX& matrix1, const XMMATRIX& matrix2, bool inDegrees = false);
XMMATRIX GetRelativeMatrix(const XMMATRIX& matrixA, const XMMATRIX& matrixB);
XMMATRIX GetRelativeRotationMatrix(const XMMATRIX& matrixA, const XMMATRIX& matrixB);

point3d quaternionToEuler(float x, float y, float z, float w);
DirectX::XMVECTOR eulerToQuanternion(float x, float y, float z);


int getRandom(int min = 1, int max = 0);
float getRandomFloat(float min, float max);
point3d getRandomDirection();

point3d rotatePoint(point3d, float);

Transform GetWorldTransform(Entity*);
Transform2D GetWorldTransform2D(Entity*);
bool IsEntityValid(Entity*);

std::vector<std::string> split(std::string, std::string);

float hueToRgb(float, float, float);
point3d hslToRgb(const point3d&);
point3d rgbToHsl(const point3d&);

point3d rotateInPlane(const point3d&, const point3d&, float);

DirectX::XMMATRIX LerpMatrix(const DirectX::XMMATRIX& from, const DirectX::XMMATRIX& to, float t);

#endif