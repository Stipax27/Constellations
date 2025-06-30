class SpiralGalaxy {
private:
    float tiltAngle;       // Наклон галактики (в градусах)
    float rotationAngle;   // Текущий угол вращения
    point3d position;      // Позиция в мировом пространстве

public:
    SpiralGalaxy() {
        tiltAngle = 45.0f;  // Начальный наклон 45 градусов
        rotationAngle = 0;
        position.x = window.width / 6;
        position.y = window.height / 6;
        position.z = 1000;
    }

    void update() {
        rotationAngle += 0.5f;  // Медленное вращение
        if (rotationAngle > 360) rotationAngle -= 360;
    }

    void draw() {
        SelectObject(window.context, fireBrush);
        SelectObject(window.context, firePen);
        srand(10);  // Фиксированный seed для одинаковой генерации

        for (int i = 0; i < 1000; i++) {
            point3d star;
            generateStar(star);  // Генерация звезды в локальных координатах

            // 1. Вращение вокруг своей оси (Z)
            star.rotateZ(star, rotationAngle);

            // 2. Наклон галактики (X)
            star.rotateX(star, tiltAngle);

            // 3. Перенос в мировые координаты
            star.x += position.x;
            star.y += position.y;
            star.z += position.z;

            // Дополнительные преобразования из вашего кода
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
