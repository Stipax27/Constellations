class SpaceCamera {
public:
    point3d position;  // Позиция в пространстве
    point3d forward;   // Направление "вперед"
    point3d right;     // Направление "вправо"
    point3d up;        // Направление "вверх"

    float moveSpeed = 0.1f;
    float rotationSpeed = 1.0f;

    SpaceCamera() {
        // Начальная ориентация - смотрим по оси Z
        position = { 0, 0, 0 };
        forward = { 0, 0, 1 };
        right = { 1, 0, 0 };
        up = { 0, 1, 0 };
    }

    // Обновление векторов направления после вращения
    void updateVectors() {
        // Нормализуем векторы
        forward = forward / sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);

        // Пересчитываем right через векторное произведение forward и up
        right = {
            forward.y * up.z - forward.z * up.y,
            forward.z * up.x - forward.x * up.z,
            forward.x * up.y - forward.y * up.x
        };
        right = right / sqrt(right.x * right.x + right.y * right.y + right.z * right.z);

        // Пересчитываем up через векторное произведение right и forward
        up = {
            right.y * forward.z - right.z * forward.y,
            right.z * forward.x - right.x * forward.z,
            right.x * forward.y - right.y * forward.x
        };
    }

    // Движение вперед/назад
    void moveForward(float amount) {
        position += forward * amount;
    }

    // Движение вправо/влево
    void moveRight(float amount) {
        position += right * amount;
    }

    // Движение вверх/вниз
    void moveUp(float amount) {
        position += up * amount;
    }

    // Вращение по оси pitch (вверх/вниз)
    void pitch(float angle) {
        float rad = angle * PI / 180.0f;
        float cosA = cos(rad);
        float sinA = sin(rad);

        point3d newForward = {
            forward.x,
            forward.y * cosA - forward.z * sinA,
            forward.y * sinA + forward.z * cosA
        };

        forward = newForward;
        updateVectors();
    }

    // Вращение по оси yaw (влево/вправо)
    void yaw(float angle) {
        float rad = angle * PI / 180.0f;
        float cosA = cos(rad);
        float sinA = sin(rad);

        point3d newForward = {
            forward.x * cosA + forward.z * sinA,
            forward.y,
            -forward.x * sinA + forward.z * cosA
        };

        forward = newForward;
        updateVectors();
    }

    // Вращение по оси roll (вращение вокруг своей оси)
    void roll(float angle) {
        float rad = angle * PI / 180.0f;
        float cosA = cos(rad);
        float sinA = sin(rad);

        point3d newRight = {
            right.x * cosA - right.y * sinA,
            right.x * sinA + right.y * cosA,
            right.z
        };

        right = newRight;
        updateVectors();
    }
};

void handleInput(SpaceCamera& camera) {
    // Движение
    if (GetKeyState('W') & 0x8000) {
        camera.moveForward(camera.moveSpeed);
    }
    if (GetKeyState('S') & 0x8000) {
        camera.moveForward(-camera.moveSpeed);
    }
    if (GetKeyState('A') & 0x8000) {
        camera.moveRight(-camera.moveSpeed);
    }
    if (GetKeyState('D') & 0x8000) {
        camera.moveRight(camera.moveSpeed);
    }
    if (GetKeyState('Q') & 0x8000) {  // Движение вниз
        camera.moveUp(-camera.moveSpeed);
    }
    if (GetKeyState('E') & 0x8000) {  // Движение вверх
        camera.moveUp(camera.moveSpeed);
    }

    // Вращение
    if (GetKeyState(VK_UP) & 0x8000) {
        camera.pitch(-camera.rotationSpeed);
    }
    if (GetKeyState(VK_DOWN) & 0x8000) {
        camera.pitch(camera.rotationSpeed);
    }
    if (GetKeyState(VK_LEFT) & 0x8000) {
        camera.yaw(-camera.rotationSpeed);
    }
    if (GetKeyState(VK_RIGHT) & 0x8000) {
        camera.yaw(camera.rotationSpeed);
    }
    if (GetKeyState('Z') & 0x8000) {  // Roll left
        camera.roll(-camera.rotationSpeed);
    }
    if (GetKeyState('X') & 0x8000) {  // Roll right
        camera.roll(camera.rotationSpeed);
    }
}

void renderScene(const SpaceCamera& camera) {
    
    RECT clientRect;
    GetClientRect(window.hWnd, &clientRect);
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); // Черная кисть
    FillRect(window.context, &clientRect, brush);
    DeleteObject(brush);

    // 2. Рендеринг звезд
    for (auto& star : stars) {
        // Переводим в систему координат камеры
        point3d viewSpace = star - camera.position;

        // Проекция (только то, что перед камерой)
        if (viewSpace.z > nearPlaneClip) {  // Используем ваш nearPlaneClip
            float scale = 1000.0f / viewSpace.z;

            point3d screenPoint = {
                window.width / 2 + viewSpace.x * scale,
                window.height / 2 - viewSpace.y * scale,
                viewSpace.z
            };

            // Размер точки зависит от расстояния
            float starSize = def_size * (1000.0f / viewSpace.z);
            starSize = std::clamp(starSize, 1.0f, def_size); // Ограничиваем размер

            // Рисуем звезду
            screenPoint.draw(screenPoint, starSize);
        }
    }
}
