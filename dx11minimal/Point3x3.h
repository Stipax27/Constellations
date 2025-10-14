#ifndef _POINT3X3_H
#define _POINT3X3_H

#include "Point3d.h"

using namespace DirectX;

struct point3x3 {
    point3d m0;
    point3d m1;
    point3d m2;

    point3x3();
    point3x3(
        float X0, float Y0, float Z0,
        float X1, float Y1, float Z1,
        float X2, float Y2, float Z2
    );
    point3x3(point3d M0, point3d M1, point3d M2);

    inline point3x3 lerp(const point3x3& other, float a);

    bool operator==(const point3x3& other) const;
    bool operator!=(const point3x3& other) const;
    point3x3& operator=(const point3x3& other);

    point3x3 operator+(const point3x3& other) const;
    point3x3 operator+(float scalar) const;
    point3x3& operator+=(const point3x3& other);
    point3x3& operator+=(float scalar);

    point3x3 operator-(const point3x3& other) const;
    point3x3 operator-(float scalar) const;
    point3x3 operator-() const;
    point3x3& operator-=(const point3x3& other);
    point3x3& operator-=(float scalar);

    point3x3 operator*(float scalar) const;
    point3x3 operator*(const point3x3& other) const;
    point3x3& operator*=(float scalar);
    point3x3& operator*=(const point3x3& other);

    point3x3 operator/(const point3x3& other) const;
    point3x3 operator/(float scalar) const;
    point3x3& operator/=(const point3x3& other);
    point3x3& operator/=(float scalar);
};

#endif