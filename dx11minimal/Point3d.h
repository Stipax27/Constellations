#ifndef _POINT3D_H
#define _POINT3D_H

#include <sstream>
#include <corecrt_math.h>

struct point3d {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    point3d();
    point3d(float X, float Y, float Z);

    float magnitude() const;
    point3d normalized() const;
    point3d cross(const point3d& other) const;
    inline point3d lerp(const point3d& other, float a);
    point3d floor3();
    point3d fract();
    float dot(const point3d& other) const;

    bool operator==(const point3d& other) const;
    bool operator!=(const point3d& other) const;
    point3d& operator=(const point3d& other);

    point3d operator+(const point3d& other) const;
    point3d operator+(float scalar) const;
    point3d& operator+=(const point3d& other);
    point3d& operator+=(float scalar);

    point3d operator-(const point3d& other) const;
    point3d operator-(float scalar) const;
    point3d operator-() const;
    point3d& operator-=(const point3d& other);
    point3d& operator-=(float scalar);

    point3d operator*(float scalar) const;
    point3d operator*(const point3d& other) const;
    point3d& operator*=(float scalar);
    point3d& operator*=(const point3d& other);

    point3d operator/(const point3d& other) const;
    point3d operator/(float scalar) const;
    point3d& operator/=(const point3d& other);
    point3d& operator/=(float scalar);
};

#endif