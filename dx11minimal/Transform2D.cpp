#include "Transform2D.h"


Transform2D& Transform2D::operator=(const Transform2D& other) {
    position = other.position;
    scale = other.scale;
    rotation = other.rotation;

    return *this;
}

Transform2D Transform2D::operator+(const Transform2D& other) {
    Transform2D tranform = Transform2D();

    //tranform.position = position + (GetRightVector() * other.position.x + GetUpVector() * other.position.y + GetLookVector() * other.position.z) * scale;
    tranform.scale = scale * other.scale;
    tranform.rotation = other.rotation + rotation;

    return tranform;
}

Transform2D& Transform2D::operator+=(const Transform2D& other) {
    //position += (GetRightVector() * other.position.x + GetUpVector() * other.position.y + GetLookVector() * other.position.z) * scale;
    scale *= other.scale;
    rotation = other.rotation + rotation;

    return *this;
}