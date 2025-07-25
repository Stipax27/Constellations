
float nearPlaneClip = 0;

constexpr float def_size = 10;

class point3d {
public:
    float x, y, z;


    point3d(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
        : x(X), y(Y), z(Z)
    {}


    float magnitude() {
        return sqrt(x * x + y * y + z * z);
    }

    inline point3d normalized() {
        float length = magnitude();
        if (length > 0)
        {
            return *this / magnitude();
        }
        else
        {
            return point3d(0.0f, 1.0f);
        }
    }

    inline point3d lerp(const point3d& other, float a)
    {
        return { lerp(x, other.x, a), lerp(y, other.y, a), lerp(z, other.z, a) };
    }


    bool operator==(const point3d& other) const {   // Перегрузка Операторов для точек.
        return fabs(x - other.x) < 0.001f &&
            fabs(y - other.y) < 0.001f &&
            fabs(z - other.z) < 0.001f;
    }
    bool operator!=(const point3d& other) const {
        return !(*this == other);
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
    
    void draw(point3d& p, float sz = def_size)
    {
        //if (p.z < nearPlaneClip) return;

        ConstBuf::ConstToVertex(4);
        ConstBuf::ConstToPixel(4);
        // Camera::Camera();

        
        ConstBuf::global[0] = XMFLOAT4(p.x, p.y, p.z, sz);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);

       Draw::elipse(1);

        //Shaders::vShader(0);
        //Shaders::pShader(0);
        //Draw::NullDrawer(0, 1);
        // Рисование элипса. sz = Размер звезды.

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

    void rotateX(point3d& p, float angle)// поворот по Оси X.
    {
        float a = angle * PI / 180.;

        float x1 = p.x;
        float y1 = p.y * cos(a) - p.z * sin(a);
        float z1 = p.y * sin(a) + p.z * cos(a);

        p.x = x1;
        p.y = y1;
        p.z = z1;
    }

    void rotateY(point3d& p, float angle)// поворот по Оси Y.
    {
        float a = angle * PI / 180.;

        float x1 = p.x * cos(a) - p.z * sin(a);
        float y1 = p.y;
        float z1 = p.x * sin(a) + p.z * cos(a);

        p.x = x1;
        p.y = y1;
        p.z = z1;
    }

    void rotateZ(point3d& p, float angle)// поворот по Оси Z.
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


point3d operator*(float scalar, const point3d& point) {
    return point * scalar;
};