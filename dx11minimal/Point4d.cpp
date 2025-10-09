#include "Point4d.h"

point4d::point4d() = default;
point4d::point4d(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

float point4d::magnitude() const {

    float xx = isnan(x) ? 0.0f : x;
    float yy = isnan(y) ? 0.0f : y;
    float zz = isnan(z) ? 0.0f : z;

    return sqrt(xx * xx + yy * yy + zz * zz);
}

point4d point4d::normalized() const {
    float mag = magnitude();
    if (mag <= std::numeric_limits<float>::epsilon()) {
        return point4d(0.0f, 1.0f, 0.0f, 0.0f);
    }
    return *this / mag;
}

point4d point4d::cross(const point4d& other) const {
    return point4d{
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    };
}

inline point4d point4d::lerp(const point4d& other, float a)
{
    return { ::lerp(x, other.x, a), ::lerp(y, other.y, a), ::lerp(z, other.z, a), ::lerp(w, other.w, a) };
}

point4d point4d::floor4()
{
    return { floor(x), floor(y), floor(z), floor(w)};
}

point4d point4d::fract()
{
    return { x - floor(x), y - floor(y), z - floor(z), w - floor(w)};
}

float point4d::dot(const point4d& other) const {
    return x * other.x + y * other.y + z * other.z + w * other.w;
}


bool point4d::operator==(const point4d& other) const {
    return fabs(x - other.x) < 0.001f &&
        fabs(y - other.y) < 0.001f &&
        fabs(z - other.z) < 0.001f &&
        fabs(w - other.w) < 0.001f;
}

bool point4d::operator!=(const point4d& other) const {
    return !(*this == other);
}

point4d& point4d::operator=(const point4d& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}


point4d point4d::operator+(const point4d& other) const {
    return { x + other.x, y + other.y, z + other.z, w + other.w };
}
point4d point4d::operator+(float scalar) const {
    return { x + scalar, y + scalar, z + scalar, w + scalar };
}
point4d& point4d::operator+=(const point4d& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}
point4d& point4d::operator+=(float scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
    w += scalar;
    return *this;
}


point4d point4d::operator-(const point4d& other) const {
    return { x - other.x, y - other.y, z - other.z, w - other.w };
}
point4d point4d::operator-(float scalar) const {
    return { x - scalar, y - scalar, z - scalar, w - scalar };
}
point4d point4d::operator-() const {
    return { -x, -y, -z, -w };
}
point4d& point4d::operator-=(const point4d& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}
point4d& point4d::operator-=(float scalar) {
    x -= scalar;
    y -= scalar;
    z -= scalar;
    w -= scalar;
    return *this;
}


point4d point4d::operator*(float scalar) const {
    return { x * scalar, y * scalar, z * scalar };
}
point4d point4d::operator*(const point4d& other) const {
    return { x * other.x, y * other.y, z * other.z };
}
point4d& point4d::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}
point4d& point4d::operator*=(const point4d& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
}


point4d point4d::operator/(const point4d& other) const {
    return { x / other.x, y / other.y, z / other.z };
}
point4d point4d::operator/(float scalar) const {
    return { x / scalar, y / scalar, z / scalar };
}
point4d& point4d::operator/=(const point4d& other) {
    x /= other.x;
    y /= other.y;
    z /= other.z;
    return *this;
}
point4d& point4d::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}