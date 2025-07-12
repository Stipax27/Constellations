point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

point3d flyDirection = { 0, 0, 0 };
point3d heroPosition = { 0, 0, 0 };
float currentFlySpeed = 0.0f;
const float maxFlySpeed = 0.1f;
const float flyAcceleration = 0.2f;
const float flyDeceleration = 0.002f;
const float turnSpeed = 0.05f; // Скорость поворота (можно настроить)


void updateFlyDirection() { // Раскладка управления 
    flyDirection = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) {
        // Добавляем вектор направления камеры (уже нормализован)
        flyDirection.x += XMVectorGetX(Hero::state.Forward) * 1.0f;
        flyDirection.y += XMVectorGetY(Hero::state.Forward) * 1.0f;
        flyDirection.z += XMVectorGetZ(Hero::state.Forward) * 1.0f;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        // Движение назад - обратное направление
        flyDirection.x -= XMVectorGetX(Hero::state.Forward) * 1.0f;
        flyDirection.y -= XMVectorGetY(Hero::state.Forward) * 1.0f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Forward) * 1.0f;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        // Добавляем вектор направления камеры (уже нормализован)
        flyDirection.x += XMVectorGetX(Hero::state.Right) * 0.5f;
        flyDirection.y += XMVectorGetY(Hero::state.Right) * 0.5f;
        flyDirection.z += XMVectorGetZ(Hero::state.Right) * 0.5f;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        // Движение назад - обратное направление
        flyDirection.x -= XMVectorGetX(Hero::state.Right) * 0.5f;
        flyDirection.y -= XMVectorGetY(Hero::state.Right) * 0.5f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Right) * 0.5f;
    }

    float dPitch = 0.0f; // Вращение вокруг оси X (вверх/вниз)
    float dYaw = 0.0f;   // Вращение вокруг оси Y (влево/вправо)
    float dRoll = 0.0f;  // Вращение вокруг оси Z (крен)

    if (GetAsyncKeyState(VK_UP)) { dPitch += turnSpeed; }
    if (GetAsyncKeyState(VK_DOWN)) { dPitch -= turnSpeed; }

    // Вращение по рысканью (Yaw - ось Y)
    if (GetAsyncKeyState(VK_LEFT)) { dYaw += turnSpeed; }
    if (GetAsyncKeyState(VK_RIGHT)) { dYaw -= turnSpeed; }

    // Вращение по крену (Roll - ось Z)
    if (GetAsyncKeyState('E')) { dRoll -= turnSpeed; }
    if (GetAsyncKeyState('Q')) { dRoll += turnSpeed; }

    if (dPitch != 0.0f || dYaw != 0.0f || dRoll != 0.0f) {
        // Создаем кватернионы для каждого вращения
        XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), dPitch);
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dYaw);
        XMVECTOR qRoll = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), dRoll);

        // Комбинируем вращения (порядок важен - обычно Yaw -> Pitch -> Roll)
        XMVECTOR qTotal = XMQuaternionMultiply(qYaw, qPitch);
        qTotal = XMQuaternionMultiply(qTotal, qRoll);

        // Применяем вращение к текущей ориентации
        Hero::state.currentRotation = XMQuaternionMultiply(qTotal, Hero::state.currentRotation);
        Hero::state.currentRotation = XMQuaternionNormalize(Hero::state.currentRotation);

        // Обновляем векторы направления
        Hero::state.Forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
        Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
        Hero::state.Right = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), Hero::state.currentRotation);
    }
    /*float length = sqrt(flyDirection.x * flyDirection.x + flyDirection.y * flyDirection.y + flyDirection.z * flyDirection.z);

    if (length > 0) {
        flyDirection.x /= length;
        flyDirection.y /= length;
        flyDirection.z /= length;
    }*/
    

}

void updateFlySpeed(float deltaTime) {// Обновления во время полёта
    bool isMoving = (flyDirection.x != 0 || flyDirection.y != 0 || flyDirection.z != 0);

    if (isMoving)
    {

        currentFlySpeed += flyAcceleration * deltaTime;

        if (currentFlySpeed > maxFlySpeed) {
            currentFlySpeed = maxFlySpeed;
        }
    }
    else {
        currentFlySpeed -= flyDeceleration * deltaTime;
        if (currentFlySpeed < 0) {
            currentFlySpeed = 0;
        }
    }
}

void updatePlayerPosition(float deltaTime) {// обновление позиции ГГ
    if (currentFlySpeed > 0) {
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) currentFlySpeed *= 5.f;// Ускорение полёта

         Camera::state.constellationOffset = Camera::state.constellationOffset *XMMatrixTranslation(flyDirection.x * currentFlySpeed * deltaTime, flyDirection.y * currentFlySpeed * deltaTime, flyDirection.z * currentFlySpeed * deltaTime);
         float x = Camera::state.constellationOffset.r[3].m128_f32[0];
         float y = Camera::state.constellationOffset.r[3].m128_f32[1];
         float z = Camera::state.constellationOffset.r[3].m128_f32[2];

         Camera::state.at = XMVectorSet(x, y, z, 0)*20;
         Camera::state.Up = XMVector3Rotate(Camera::state.defaultUp, Camera::state.currentRotation);
         Camera::state.Forward = XMVector3Normalize(Camera::state.at - Camera::state.Eye);
         Camera::state.Eye = Camera::state.at - (Camera::state.Forward * Camera::state.camDist);
         Camera::Camera();
    }

}
