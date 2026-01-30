#include "Transform2D.h"


point3d Transform2D::GetRightVector()
{
    return point3d(cos(rotation), -sin(rotation), 0);
}

point3d Transform2D::GetUpVector()
{
    return point3d(sin(rotation), cos(rotation), 0);
}


Transform2D& Transform2D::operator=(const Transform2D& other) {
    position = other.position;
    anchorPoint = other.position;
    scale = other.scale;
    rotation = other.rotation;
    ratio = other.ratio;

    return *this;
}

Transform2D Transform2D::operator+(const Transform2D& other) {
    Transform2D transform2d = Transform2D();

    transform2d.position = position + (GetRightVector() * other.position.x + GetUpVector() * other.position.y) * scale;
    transform2d.anchorPoint = other.anchorPoint;
    transform2d.scale = scale * other.scale;
    transform2d.rotation = other.rotation + rotation;
    transform2d.ratio = other.ratio;

    return transform2d;
}

Transform2D& Transform2D::operator+=(const Transform2D& other) {
    position += (GetRightVector() * other.position.x + GetUpVector() * other.position.y) * scale;
    anchorPoint = other.anchorPoint;
    scale *= other.scale;
    rotation = other.rotation + rotation;
    ratio = other.ratio;

    return *this;
}