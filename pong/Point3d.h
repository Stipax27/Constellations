
float nearPlaneClip = 0;

constexpr float def_size = 10;

class point3d {
public:
    float x, y, z;
    
    bool operator==(const point3d& other) const {   // ���������� ���������� ��� �����.
        return fabs(x - other.x) < 0.001f &&
            fabs(y - other.y) < 0.001f &&
            fabs(z - other.z) < 0.001f;
    }
    bool operator!=(const point3d& other) const {
        return !(*this == other);
    }
    point3d operator+(const point3d& other) const {
        return point3d{ x + other.x, y + other.y, z + other.z };
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
        return point3d{ x - other.x, y - other.y, z - other.z };
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
        return point3d{ x * scalar, y * scalar, z * scalar };
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
    point3d operator/(float scalar) const {
        return point3d{ x / scalar, y / scalar, z / scalar };
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
      
    void draw(point3d& p, float sz = def_size)
    {
        if (p.z < nearPlaneClip) return;

        Ellipse(window.context,
            p.x - sz,
            p.y - sz,
            p.x + sz,
            p.y + sz
        );// ��������� ������. sz = ������ ������.

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
};

point3d operator*(float scalar, const point3d& point) {
    return point * scalar;
};
