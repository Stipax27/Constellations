#include "math.h"
#include "utils.h"

class point4d {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    // Конструкторы
    point4d() = default;
    point4d(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
    explicit point4d(float val) : x(val), y(val), z(val), w(val) {}

    float magnitude() const {

        float xx = isnan(x) ? 0.0f : x;
        float yy = isnan(y) ? 0.0f : y;
        float zz = isnan(z) ? 0.0f : z;
        float ww = isnan(w) ? 0.0f : w;

        return sqrt(xx * xx + yy * yy + zz * zz + ww * ww);
    }

    point4d normalized() const {
        float mag = magnitude();
        if (mag == 0) {
            return point4d(0.0f, 1.0f, 0.0f, 0.0f);
        }
        return *this / mag;
    }

    point4d cross(const point4d& other) const {
        return point4d{
            x * other.y - y * other.x,
            y * other.z - z * other.y,
            z * other.w - w * other.z,
            w * other.x - x * other.w
        };
    }

    inline point4d lerp(const point4d& other, float a)
    {
        return { lerp(x, other.x, a), lerp(y, other.y, a), lerp(z, other.z, a), lerp(w, other.w, a) };
    }

    point4d floor4()
    {
        return { floor(x), floor(y), floor(z), floor(w) };
    }

    point4d fract()
    {
        return { x - floor(x), y - floor(y), z - floor(z), w - floor(w) };
    }


    bool operator==(const point4d& other) const {   // ���������� ���������� ��� �����.
        return x == other.x &&
            y == other.y &&
            z == other.z &&
            w == other.w;
    }

    bool operator!=(const point4d& other) const {
        return !(*this == other);
    }


    point4d operator+(const point4d& other) const {
        return { x + other.x, y + other.y, z + other.z, w + other.w };
    }
    point4d operator+(float scalar) const {
        return { x + scalar, y + scalar, z + scalar, w + scalar };
    }
    point4d& operator+=(const point4d& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    point4d& operator+=(float scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }


    point4d operator-(const point4d& other) const {
        return { x - other.x, y - other.y, z - other.z, w - other.w };
    }
    point4d operator-(float scalar) const {
        return { x - scalar, y - scalar, z - scalar, w - scalar };
    }
    point4d operator-() const {
        return { -x, -y, -z, -w };
    }
    point4d& operator-=(const point4d& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    point4d& operator-=(float scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }


    point4d operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }
    point4d operator*(const point4d& other) const {
        return { x * other.x, y * other.y, z * other.z, w * other.w };
    }
    point4d& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
    point4d& operator*=(const point4d& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }


    point4d operator/(const point4d& other) const {
        return { x / other.x, y / other.y, z / other.z, w / other.w };
    }
    point4d operator/(float scalar) const {
        return { x / scalar, y / scalar, z / scalar, w / scalar };
    }
    point4d& operator/=(const point4d& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }
    point4d& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }
    float Dot(const point4d& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }


    void move(float _x, float _y, float _z, float _w)
    {
        x += _x;
        y += _y;
        z += _z;
        w += _w;
    }


    void move(point4d& p, float x, float y, float z, float w)
    {
        p.x += x;
        p.y += y;
        p.z += z;
        p.w += w;
    }

    void move(point4d& p, const point4d& ofs) {
        p += ofs;
    }

    static float lerp(float x1, float x2, float a)
    {
        return x1 * (1 - a) + x2 * a;
    }

    float dot(const point4d& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

};