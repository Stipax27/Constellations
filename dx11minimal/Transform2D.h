#ifndef _TRANSFORM_2D_H_
#define _TRANSFORM_2D_H_

#include "component.h"
#include "point3d.h"

enum ScreenAspectRatio {
	XY,
	YX,
	XX,
	YY
};

struct Transform2D : Component
{
	point3d position = point3d();
	point3d anchorPoint = point3d();
	point3d scale = point3d(1.0f, 1.0f, 0.0f);

	float rotation = 0.0f;
	ScreenAspectRatio ratio = ScreenAspectRatio::XY;

	point3d GetRightVector();
	point3d GetUpVector();

	Transform2D& operator=(const Transform2D&);
	Transform2D operator+(const Transform2D&);
	Transform2D& operator+=(const Transform2D&);
};

#endif