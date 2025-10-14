#include "Point3x3.h"

point3x3::point3x3() {
    m0 = point3d();
    m1 = point3d();
    m2 = point3d();
}
point3x3::point3x3(
    float X0, float Y0, float Z0,
    float X1, float Y1, float Z1,
    float X2, float Y2, float Z2
) {
    m0 = point3d(X0, Y0, Z0);
    m1 = point3d(X1, Y1, Z1);
    m2 = point3d(X2, Y2, Z2);
}
point3x3::point3x3(point3d M0, point3d M1, point3d M2) : m0(M0), m1(M1), m2(M2)
{}


inline point3x3 point3x3::lerp(const point3x3& other, float a)
{
    return { m0.lerp(other.m0, a),
        m1.lerp(other.m1, a),
        m2.lerp(other.m2, a) };
}


bool point3x3::operator==(const point3x3& other) const {
    return m0 == other.m0 &&
        m1 == other.m1 &&
        m2 == other.m2;
}

bool point3x3::operator!=(const point3x3& other) const {
    return !(*this == other);
}

point3x3& point3x3::operator=(const point3x3& other) {
    m0 = other.m0;
    m1 = other.m1;
    m2 = other.m2;
    return *this;
}


//point3x3 point3x3::operator+(const point3x3& other) const {
//    return { x + other.x, y + other.y, z + other.z };
//}
//point3x3 point3x3::operator+(float scalar) const {
//    return { x + scalar, y + scalar, z + scalar };
//}
//point3x3& point3x3::operator+=(const point3x3& other) {
//    x += other.x;
//    y += other.y;
//    z += other.z;
//    return *this;
//}
//point3x3& point3x3::operator+=(float scalar) {
//    x += scalar;
//    y += scalar;
//    z += scalar;
//    return *this;
//}
//
//
//point3x3 point3x3::operator-(const point3x3& other) const {
//    return { x - other.x, y - other.y, z - other.z };
//}
//point3x3 point3x3::operator-(float scalar) const {
//    return { x - scalar, y - scalar, z - scalar };
//}
//point3x3 point3x3::operator-() const {
//    return { -x, -y, -z };
//}
//point3x3& point3x3::operator-=(const point3x3& other) {
//    x -= other.x;
//    y -= other.y;
//    z -= other.z;
//    return *this;
//}
//point3x3& point3x3::operator-=(float scalar) {
//    x -= scalar;
//    y -= scalar;
//    z -= scalar;
//    return *this;
//}


point3x3 point3x3::operator*(float scalar) const {
    return { m0 * scalar, m1 * scalar, m2 * scalar };
}
point3x3 point3x3::operator*(const point3x3& other) const {
    return {
        m0.x * other.m0.x + m0.y * other.m1.x + m0.z * other.m2.x,
        m0.x * other.m0.y + m0.y * other.m1.y + m0.z * other.m2.y, 
        m0.x * other.m0.z + m0.y * other.m1.z + m0.z * other.m2.z,
        
        m1.x * other.m0.x + m1.y * other.m1.x + m1.z * other.m2.x,
        m1.x * other.m0.y + m1.y * other.m1.y + m1.z * other.m2.y,
        m1.x * other.m0.z + m1.y * other.m1.z + m1.z * other.m2.z,
        
        m2.x * other.m0.x + m2.y * other.m1.x + m2.z * other.m2.x,
        m2.x * other.m0.y + m2.y * other.m1.y + m2.z * other.m2.y,
        m2.x * other.m0.z + m2.y * other.m1.z + m2.z * other.m2.z
    };
}
point3x3& point3x3::operator*=(float scalar) {
    m0 *= scalar;
    m1 *= scalar;
    m2 *= scalar;
    return *this;
}
point3x3& point3x3::operator*=(const point3x3& other) {
    m0.x = m0.x * other.m0.x + m0.y * other.m1.x + m0.z * other.m2.x;
    m0.y = m0.x * other.m0.y + m0.y * other.m1.y + m0.z * other.m2.y;
    m0.z = m0.x * other.m0.z + m0.y * other.m1.z + m0.z * other.m2.z;
         
    m1.x = m1.x * other.m0.x + m1.y * other.m1.x + m1.z * other.m2.x;
    m1.y = m1.x * other.m0.y + m1.y * other.m1.y + m1.z * other.m2.y;
    m1.z = m1.x * other.m0.z + m1.y * other.m1.z + m1.z * other.m2.z;
         
    m2.x = m2.x * other.m0.x + m2.y * other.m1.x + m2.z * other.m2.x;
    m2.y = m2.x * other.m0.y + m2.y * other.m1.y + m2.z * other.m2.y;
    m2.z = m2.x * other.m0.z + m2.y * other.m1.z + m2.z * other.m2.z;

    return *this;
}


//point3x3 point3x3::operator/(const point3x3& other) const {
//    return { x / other.x, y / other.y, z / other.z };
//}
//point3x3 point3x3::operator/(float scalar) const {
//    return { x / scalar, y / scalar, z / scalar };
//}
//point3x3& point3x3::operator/=(const point3x3& other) {
//    x /= other.x;
//    y /= other.y;
//    z /= other.z;
//    return *this;
//}
//point3x3& point3x3::operator/=(float scalar) {
//    x /= scalar;
//    y /= scalar;
//    z /= scalar;
//    return *this;
//}