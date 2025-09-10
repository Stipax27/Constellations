#pragma once

#include <sstream>
#include "DirectXMath.h"

using namespace DirectX;

class point3d {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    // Конструкторы
    point3d() = default;
    point3d(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
    explicit point3d(float val) : x(val), y(val), z(val) {}


    float magnitude() const {
        
        float xx = isnan(x) ? 0.0f : x;
        float yy = isnan(y) ? 0.0f : y;
        float zz = isnan(z) ? 0.0f : z;

        return sqrt(xx * xx + yy * yy + zz * zz);
    }

    // Улучшенный normalized
    point3d normalized() const {
        float mag = magnitude();
        if (mag <= std::numeric_limits<float>::epsilon()) {
            return point3d(0.0f, 1.0f, 0.0f); 
        }
        return *this / mag;
    }

    point3d cross(const point3d& other) const {
        return point3d{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x 
        };
    }

    inline point3d lerp(const point3d& other, float a)
    {
        return { lerp(x, other.x, a), lerp(y, other.y, a), lerp(z, other.z, a) };
    }

    point3d floor3()
    {
        return { floor(x), floor(y), floor(z) };
    }

    point3d fract()
    {
        return { x - floor(x), y - floor(y), z - floor(z) };
    }


    bool operator==(const point3d& other) const {   // ���������� ���������� ��� �����.
        return fabs(x - other.x) < 0.001f &&
            fabs(y - other.y) < 0.001f &&
            fabs(z - other.z) < 0.001f;
    }

    bool operator!=(const point3d& other) const {
        return !(*this == other);
    }

    point3d& operator=(const point3d& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }


    point3d operator+(const point3d& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }
    point3d operator+(float scalar) const {
        return { x + scalar, y + scalar, z + scalar };
    }
    point3d& operator+=(const point3d& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    point3d& operator+=(float scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }


    point3d operator-(const point3d& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }
    point3d operator-(float scalar) const {
        return { x - scalar, y - scalar, z - scalar };
    }
    point3d operator-() const {
        return { -x, -y, -z };
    }
    point3d& operator-=(const point3d& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    point3d& operator-=(float scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }


    point3d operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }
    point3d operator*(const point3d& other) const {
        return { x * other.x, y * other.y, z * other.z };
    }
    point3d& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    point3d& operator*=(const point3d& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }


    point3d operator/(const point3d& other) const {
        return { x / other.x, y / other.y, z / other.z };
    }
    point3d operator/(float scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }
    point3d& operator/=(const point3d& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }
    point3d& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    float Dot(const point3d& other) const {
        return x * other.x + y * other.y + z * other.z;
    }


    void move(float _x, float _y, float _z)
    {
        x += _x;
        y += _y;
        z += _z;
    }


    void move(point3d& p, float x, float y, float z)
    {
        p.x += x;
        p.y += y;
        p.z += z;
    }

    void move(point3d& p, const point3d& ofs) {
        p += ofs;
    }

    void rotateX(point3d& p, float angle)// ������� �� ��� X.
    {
        float a = angle * PI / 180.;

        float x1 = p.x;
        float y1 = p.y * cos(a) - p.z * sin(a);
        float z1 = p.y * sin(a) + p.z * cos(a);

        p.x = x1;
        p.y = y1;
        p.z = z1;
    }

    void rotateY(point3d& p, float angle)// ������� �� ��� Y.
    {
        float a = angle * PI / 180.;

        float x1 = p.x * cos(a) - p.z * sin(a);
        float y1 = p.y;
        float z1 = p.x * sin(a) + p.z * cos(a);

        p.x = x1;
        p.y = y1;
        p.z = z1;
    }

    void rotateZ(point3d& p, float angle)// ������� �� ��� Z.
    {
        float a = angle * PI / 180.;

        float x1 = p.x * cos(a) - p.y * sin(a);
        float y1 = p.x * sin(a) + p.y * cos(a);
        float z1 = p.z;

        p.x = x1;
        p.y = y1;
        p.z = z1;
    }

    static float lerp(float x1, float x2, float a)
    {
        return x1 * (1 - a) + x2 * a;
    }

    static point3d lerp(point3d p1, point3d p2, float a)
    {
        return { lerp(p1.x, p2.x, a), lerp(p1.y, p2.y, a), lerp(p1.z, p2.z, a) };
    }

    float DistanceTo(const point3d& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    float dot(const point3d& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};

point3d TransformPoint(const point3d& p, const XMMATRIX& m) {
    XMVECTOR v = XMVectorSet(p.x, p.y, p.z, 1.0f);
    v = XMVector3Transform(v, m);
    return {
        XMVectorGetX(v),
        XMVectorGetY(v),
        XMVectorGetZ(v)
    };
}