#ifndef _POINT4D_H
#define _POINT4D_H

#include <sstream>
#include "DirectXMath.h"
#include "utils.h"

using namespace DirectX;

struct point4d {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    point4d();
    point4d(float X, float Y, float Z, float W);

    float magnitude() const;
    point4d normalized() const;
    point4d cross(const point4d& other) const;
    inline point4d lerp(const point4d& other, float a);
    point4d floor4();
    point4d fract();
    float dot(const point4d& other) const;

    bool operator==(const point4d& other) const;
    bool operator!=(const point4d& other) const;
    point4d& operator=(const point4d& other);

    point4d operator+(const point4d& other) const;
    point4d operator+(float scalar) const;
    point4d& operator+=(const point4d& other);
    point4d& operator+=(float scalar);

    point4d operator-(const point4d& other) const;
    point4d operator-(float scalar) const;
    point4d operator-() const;
    point4d& operator-=(const point4d& other);
    point4d& operator-=(float scalar);

    point4d operator*(float scalar) const;
    point4d operator*(const point4d& other) const;
    point4d& operator*=(float scalar);
    point4d& operator*=(const point4d& other);

    point4d operator/(const point4d& other) const;
    point4d operator/(float scalar) const;
    point4d& operator/=(const point4d& other);
    point4d& operator/=(float scalar);
};

#endif