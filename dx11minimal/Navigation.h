point3d flyDirection = point3d(0, 0, 1);
point3d flyRightDirection = point3d(1, 0, 0);
point3d flyUpDirection = point3d(0, 1, 0);

float currentFlySpeed = 10.0f;
float boostingFlySpeed;
const float maxFlySpeed = 15.f;
const float MOUSE_SENSITIVITY = 0.002f;
const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.45f;
const float SENSIVITY = 0.11f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;

void getPerpendicularDirections()
{
    point3d world_up = point3d(0, 1, 0);

    if ((flyDirection - world_up).magnitude() < 0.001f || (flyDirection + world_up).magnitude() < 0.001f) {
        world_up = point3d(1, 0, 0);
    }

    flyRightDirection = (flyDirection.cross(world_up)).normalized();
    flyUpDirection = (flyRightDirection.cross(flyDirection)).normalized();
}

void updateFlyDirection()
{
    if (currentFlySpeed > maxFlySpeed)
    {
        return;
    }

    point3d Direction = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) {
        Direction.x += XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        Direction.y += XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        Direction.z += XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        Direction.x -= XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        Direction.y -= XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        Direction.z -= XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        Direction.x -= XMVectorGetX(Hero::state.Right) * 5.f;
        Direction.y -= XMVectorGetY(Hero::state.Right) * 5.f;
        Direction.z -= XMVectorGetZ(Hero::state.Right) * 5.f;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        Direction.x += XMVectorGetX(Hero::state.Right) * 5.f;
        Direction.y += XMVectorGetY(Hero::state.Right) * 5.f;
        Direction.z += XMVectorGetZ(Hero::state.Right) * 5.f;
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        Direction.x += XMVectorGetX(Hero::state.Up) * 5.f;
        Direction.y += XMVectorGetY(Hero::state.Up) * 5.f;
        Direction.z += XMVectorGetZ(Hero::state.Up) * 5.f;
    }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
        Direction.x -= XMVectorGetX(Hero::state.Up) * 5.f;
        Direction.y -= XMVectorGetY(Hero::state.Up) * 5.f;
        Direction.z -= XMVectorGetZ(Hero::state.Up) * 5.f;
    }

    if (Direction.magnitude() > 0)
    {
        Direction = Direction.normalized();
        getPerpendicularDirections();
    }
    flyDirection = flyDirection.lerp(Direction, 0.1f);

    // РЕЖИМ ТАРГЕТИНГА - ГЕРОЙ ВСЕГДА СМОТРИТ НА ВРАГА, НО МОЖНО ПРИЦЕЛИВАТЬСЯ
    if (CameraTargeting::IsTargeting())
    {
        XMVECTOR enemyPos = CameraTargeting::GetTargetPosition();
        XMVECTOR heroPos = Hero::state.position;

        // ВЫЧИСЛЯЕМ НАПРАВЛЕНИЕ К ВРАГУ
        XMVECTOR toEnemy = XMVector3Normalize(enemyPos - heroPos);

        // СОЗДАЕМ КВАРТЕРНИОН ДЛЯ ПОВОРОТА К ВРАГУ
        XMVECTOR targetForward = toEnemy;
        XMVECTOR upVector = XMVectorSet(0, 1, 0, 0);

        // ИСПОЛЬЗУЕМ LookAt для создания правильного поворота
        XMMATRIX lookAtMatrix = XMMatrixLookAtLH(heroPos, enemyPos, upVector);
        XMMATRIX rotationMatrix = XMMatrixInverse(nullptr, lookAtMatrix);
        XMVECTOR targetRotation = XMQuaternionRotationMatrix(rotationMatrix);

        // ПЛАВНАЯ ИНТЕРПОЛЯЦИЯ ПОВОРОТА
        float rotationSpeed = 5.0f * (deltaTime / 1000.0f);
        Hero::state.currentRotation = XMQuaternionSlerp(
            Hero::state.currentRotation,
            targetRotation,
            rotationSpeed
        );
        Hero::state.currentRotation = XMQuaternionNormalize(Hero::state.currentRotation);

        // ОБНОВЛЯЕМ ВЕКТОРЫ ГЕРОЯ
        Hero::state.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
        Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
        Hero::state.Right = XMVector3Cross(Hero::state.Up, Hero::state.Forwardbuf);

        // НЕ ЦЕНТРИРУЕМ КУРСОР - ПОЗВОЛЯЕМ ИГРОКУ ПРИЦЕЛИВАТЬСЯ
        // Мышка теперь используется только для прицеливания атак, а не для поворота героя

        Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
    }
    else
    {
        // ОБЫЧНЫЙ РЕЖИМ - ПОЛНОЕ УПРАВЛЕНИЕ МЫШЬЮ
        float dPitch = 0.0f, dYaw = 0.0f, dRoll = 0.0f;

        float x = mouse.pos.x - window.width / 2;
        float y = mouse.pos.y - window.height / 2;

        point3d mousePos = point3d(x / window.width / aspect, y / window.height, 0);
        float length = mousePos.magnitude();

        if (length > CURSOR_IGNORE_ZONE)
        {
            if (length > MAX_CURSOR_DEVIATION)
            {
                mousePos = mousePos.normalized() * MAX_CURSOR_DEVIATION;
                SetCursorPos(mousePos.x * window.width * aspect + window.width / 2, mousePos.y * window.height + window.height / 2);
            }

            float k = (length - CURSOR_IGNORE_ZONE) / MAX_CURSOR_DEVIATION;

            float dx = (mousePos.x) * SENSIVITY * k;
            float dy = (mousePos.y) * SENSIVITY * k;

            dPitch = dy;
            dYaw = dx;
        }

        // УПРАВЛЕНИЕ Q/E ДЛЯ ВРАЩЕНИЯ
        if (GetAsyncKeyState('E')) {
            dRoll -= turnSpeed;
            Camera::state.n = lerp(Camera::state.n, 100, 0.3f);
        }
        if (GetAsyncKeyState('Q')) {
            dRoll += turnSpeed;
            Camera::state.n = lerp(Camera::state.n, 100, 0.3f);
        }

        // ПРИМЕНЯЕМ ВРАЩЕНИЕ К ГЕРОЮ
        if (dPitch != 0.0f || dYaw != 0.0f || dRoll != 0.0f) {
            XMVECTOR qPitch = XMQuaternionRotationAxis(Hero::state.Right, dPitch);
            XMVECTOR qYaw = XMQuaternionRotationAxis(Hero::state.Up, dYaw);
            XMVECTOR qRoll = XMQuaternionRotationAxis(Hero::state.Forwardbuf, dRoll);

            XMVECTOR qTotal = XMQuaternionMultiply(qYaw, qPitch);
            qTotal = XMQuaternionMultiply(qTotal, qRoll);

            Hero::state.currentRotation = XMQuaternionMultiply(Hero::state.currentRotation, qTotal);
            Hero::state.currentRotation = XMQuaternionNormalize(Hero::state.currentRotation);

            // ОБНОВЛЯЕМ ВЕКТОРЫ ГЕРОЯ
            Hero::state.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
            Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
            Hero::state.Right = XMVector3Cross(Hero::state.Up, Hero::state.Forwardbuf);
        }
        else {
            Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
        }
    }
}
int acc = 0;
bool wasShiftPressed = false;
float speed;
float localTime;
float localDeltaTime;

// ДОБАВИМ ПЕРЕМЕННЫЕ ДЛЯ УЛУЧШЕННОГО ЭФФЕКТА КАМЕРЫ
enum CameraState {
    CAMERA_NORMAL,
    CAMERA_PULLING_BACK,    // плавное отдаление
    CAMERA_CATCHING_UP      // плавное догоняние
};
CameraState cameraState = CAMERA_NORMAL;

const float MAX_CAMERA_DISTANCE = 10000.0f; // максимальное отдаление
const float NORMAL_CAMERA_DISTANCE = Camera::state.distanceOffset; // обычная дистанция
const float PULL_BACK_SPEED = 0.55f; // скорость отдаления
const float CATCH_UP_SPEED = 0.08f;  // скорость догоняния
float cameraEffectStartTime = 0;
const float CAMERA_EFFECT_DURATION = 1500.0f; // длительность эффекта в ms

static DWORD lastRushTime = 0;
const DWORD RUSH_COOLDOWN = 1000;
bool canRush = true;

LONG energy = 300;

void updateFlySpeed(float deltaTime)
{
    bool isBoosting = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

    // ВОССТАНОВЛЕНИЕ ЭНЕРГИИ
    if (energy < 300 && currentFlySpeed < 20 && !isBoosting) {
        energy += LONG((deltaTime / 17.f));
        energy = min(energy, 300L);
    }

    drawString((std::to_string(energy)).c_str(), (1250.f / 2560) * window.width, (1190.f / 1440) * window.height, .5f, false);
    drawString("Energy", (1100.f / 2560)* window.width, (1190.f / 1440)* window.height, .5f, false);

    
    deltaTime /= 1000;

    // ПРОВЕРКА КУЛДАУНА РЫВКА
    if (energy <= 0 && currentTime - lastRushTime > RUSH_COOLDOWN) {
        canRush = true;
        energy = 0;
    }

    // ОБНОВЛЕНИЕ ЭФФЕКТА КАМЕРЫ
    if (cameraState != CAMERA_NORMAL) {
        float effectProgress = (currentTime - cameraEffectStartTime) / CAMERA_EFFECT_DURATION;

        if (cameraState == CAMERA_PULLING_BACK) {
            // ФАЗА 1: ПЛАВНОЕ ОТДАЛЕНИЕ КАМЕРЫ
            float pullProgress = min(effectProgress * 2.0f, 1.0f); // быстрее отдаляем
            Camera::state.distanceOffset = lerp(NORMAL_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE, pullProgress);

            // Переходим ко второй фазе когда достаточно отдалились
            if (effectProgress > 0.3f) {
                cameraState = CAMERA_CATCHING_UP;
            }
        }
        else if (cameraState == CAMERA_CATCHING_UP) {
            // ФАЗА 2: ПЛАВНОЕ ДОГОНЯНИЕ КАМЕРЫ
            float catchProgress = max(0.0f, (effectProgress - 0.3f) / 0.7f); // нормализуем прогресс
            Camera::state.distanceOffset = lerp(MAX_CAMERA_DISTANCE, NORMAL_CAMERA_DISTANCE, catchProgress);

            // Завершаем эффект когда камера вернулась
            if (effectProgress >= 1.0f) {
                cameraState = CAMERA_NORMAL;
                Camera::state.distanceOffset = NORMAL_CAMERA_DISTANCE;
            }
        }
    }

    // СИСТЕМА РЫВКА ПРИ НУЛЕВОЙ ЭНЕРГИИ
    if (energy < 1) {
        if (isBoosting && !wasShiftPressed && canRush) {
            // НАЧАЛО НАКОПЛЕНИЯ СКОРОСТИ ДЛЯ РЫВКА
            wasShiftPressed = true;
            speed = 0;
            currentFlySpeed = 5;
            acc = 50;
            localTime = currentTime;
        }

        if (isBoosting && wasShiftPressed && canRush) {
            // НАКОПЛЕНИЕ СКОРОСТИ
            speed += acc * deltaTime;
            speed = min(speed, 25.0f);

            // ЛЕГКОЕ ОТДАЛЕНИЕ КАМЕРЫ ПРИ ЗАРЯДКЕ
            if (cameraState == CAMERA_NORMAL) {
                Camera::state.distanceOffset = lerp(Camera::state.distanceOffset, MAX_CAMERA_DISTANCE * 0.3f, 0.1f);
            }
        }

        if (!isBoosting && wasShiftPressed && canRush) {
            // АКТИВАЦИЯ РЫВКА
            wasShiftPressed = false;
            canRush = false;
            lastRushTime = currentTime;
           

            // ЗАПУСКАЕМ ЭФФЕКТ КАМЕРЫ С ДВУМЯ ФАЗАМИ
            cameraState = CAMERA_PULLING_BACK;
            cameraEffectStartTime = currentTime;

            float rushSpeed = 80 + speed;
            currentFlySpeed = min(rushSpeed, 120.0f);

            acc = -30;
            localTime = currentTime;
            speed = 0;
        }

        // БАЗОВОЕ ДВИЖЕНИЕ БЕЗ ЭНЕРГИИ
        if (!isBoosting && !wasShiftPressed) {
            currentFlySpeed = max(currentFlySpeed + acc * deltaTime, 8.0f);

            if (currentFlySpeed > 50) {
                acc = -50;
            }
        }

        return;
    }

    // СТАНДАРТНАЯ СИСТЕМА С ЭНЕРГИЕЙ
    if (isBoosting && !wasShiftPressed) {
        // НАЧАЛО БУСТА
        wasShiftPressed = true;
        speed = 0;
        currentFlySpeed = 10;
        acc = 40;
        localTime = currentTime;

        
    }

    if (isBoosting && wasShiftPressed) {
        // НАКОПЛЕНИЕ СКОРОСТИ И РАСХОД ЭНЕРГИИ
        speed += acc * deltaTime;
        speed = min(speed, 35.0f);
        energy -= 1.5;
        energy = max(energy, 0L);

        currentFlySpeed = max(currentFlySpeed * 0.9, 5.0f);
    }

    if (!isBoosting && wasShiftPressed) {
        // ОТПУСТИЛИ SHIFT - активируем накопленную скорость
        
        wasShiftPressed = false;
        localDeltaTime = currentTime - localTime;

        boostingFlySpeed = 10 + min(speed, 35.0f);
        currentFlySpeed = min(boostingFlySpeed, 60.0f);
        acc = -40;

        // ЗАПУСКАЕМ ЭФФЕКТ КАМЕРЫ ДЛЯ ОБЫЧНОГО БУСТА
        if (cameraState == CAMERA_NORMAL) {
            cameraState = CAMERA_PULLING_BACK;
            cameraEffectStartTime = currentTime;
        }

        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Whoosh.wav");
        speed = 0;
    }

    // ОБЫЧНОЕ ДВИЖЕНИЕ БЕЗ SHIFT
    if (!isBoosting && !wasShiftPressed) {
        currentFlySpeed = max(currentFlySpeed + acc * deltaTime, maxFlySpeed);

        if (currentFlySpeed > 40) {
            acc = -30;
        }
    }
}

struct EnergyCost {
    float fists = 15.0f;
    float sword = 35.0f;
    float shield = 35.0f;
    float bow = 50.0f;
    float chargeMultiplier = 2.0f; // множитель при зарядке
    float shieldBlock = 8.0f;
} energyCost;

void updatePlayerPosition(float deltaTime)
{
    if (currentFlySpeed > 0)
    {
        XMVECTOR moveDir;

        // РАЗНЫЕ НАПРАВЛЕНИЯ ДВИЖЕНИЯ В ЗАВИСИМОСТИ ОТ РЕЖИМА
        if (CameraTargeting::IsTargeting()) {
            // ПРИ ТАРГЕТИНГЕ - ДВИЖЕНИЕ ОТНОСИТЕЛЬНО КАМЕРЫ
            moveDir = XMVectorSet(flyDirection.x, flyDirection.y, flyDirection.z, 0.0f);
        }
        else {
            // ОБЫЧНЫЙ РЕЖИМ - ДВИЖЕНИЕ ОТНОСИТЕЛЬНО ГЕРОЯ
            moveDir = XMVectorSet(flyDirection.x, flyDirection.y, flyDirection.z, 0.0f);
        }

        // 2. Рассчитываем вектор смещения
        XMVECTOR displacement = XMVectorScale(moveDir, currentFlySpeed * deltaTime);

        // 3. Обновляем позицию героя
        Hero::state.position = XMVectorAdd(Hero::state.position, displacement);

        // 4. Обновляем матрицу мира
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(Hero::state.currentRotation);
        Hero::state.worldMatrix = rotationMatrix * XMMatrixTranslationFromVector(Hero::state.position);

        // 5. Обновляем constellationOffset
        Hero::state.constellationOffset = XMMatrixTranslationFromVector(Hero::state.position);
    }
}
