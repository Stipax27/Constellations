class SpiralGalaxy {
private:
    float tiltAngle;       // ������ ��������� (� ��������)
    float rotationAngle;   // ������� ���� ��������
    point3d position;      // ������� � ������� ������������

public:
    SpiralGalaxy() {
        tiltAngle = 45.0f;  // ��������� ������ 45 ��������
        rotationAngle = 0;
        position.x = window.width / 6;
        position.y = window.height / 6;
        position.z = 1000;
    }

    void update() {
        rotationAngle += 0.5f;  // ��������� ��������
        if (rotationAngle > 360) rotationAngle -= 360;
    }

    void draw() {
        SelectObject(window.context, fireBrush);
        SelectObject(window.context, firePen);
        srand(10);  // ������������� seed ��� ���������� ���������

        for (int i = 0; i < 1000; i++) {
            point3d star;
            generateStar(star);  // ��������� ������ � ��������� �����������

            // 1. �������� ������ ����� ��� (Z)
            star.rotateZ(star, rotationAngle);

            // 2. ������ ��������� (X)
            star.rotateX(star, tiltAngle);

            // 3. ������� � ������� ����������
            star.x += position.x;
            star.y += position.y;
            star.z += position.z;

            // �������������� �������������� �� ������ ����
            modelTransform(star, Aries);
            modelProject(star);

            star.draw(star, 2);
        }
    }

private:
    void generateStar(point3d& p) {
        float diskRadius = 300.0f;
        float coreRadius = 50.0f;
        float thickness = 30.0f;
        float spiralTightness = 2.5f;

        float distance = coreRadius + (rand() % (int)(diskRadius - coreRadius));
        float angle = rand() % 360;
        float height = (rand() % (int)thickness) - thickness / 2;

        angle += spiralTightness * (distance / diskRadius) * 360;

        p.x = distance * cos(angle * PI / 180.);
        p.y = distance * sin(angle * PI / 180.);
        p.z = height;
    }
};
