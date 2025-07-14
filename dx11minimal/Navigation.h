point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

point3d flyDirection = { 0, 0, 0 };
point3d heroPosition = { 0, 0, 0 };
float currentFlySpeed = 0.0f;
const float maxFlySpeed = 0.1f;
const float flyAcceleration = 0.2f;
const float flyDeceleration = 0.002f;


void updateFlyDirection() { // Раскладка управления 
    flyDirection = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) {
        // Добавляем вектор направления камеры (уже нормализован)
        flyDirection.x += XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.y += XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.z += XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        // Движение назад - обратное направление
        flyDirection.x -= XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.y -= XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        // Добавляем вектор направления камеры (уже нормализован)
        flyDirection.x -= XMVectorGetX(Hero::state.Right) * 5.f;
        flyDirection.y -= XMVectorGetY(Hero::state.Right) * 5.f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Right) * 5.f;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        // Движение назад - обратное направление
        flyDirection.x += XMVectorGetX(Hero::state.Right) * 5.f;
        flyDirection.y += XMVectorGetY(Hero::state.Right) * 5.f;
        flyDirection.z += XMVectorGetZ(Hero::state.Right) * 5.f;
    }

    float dPitch = 0.0f, dYaw = 0.0f, dRoll = 0.0f;

    if (GetAsyncKeyState(VK_UP)) {
        dPitch += turnSpeed; Camera::state.n += 10;
    }
    if (GetAsyncKeyState(VK_DOWN)){
        dPitch -= turnSpeed; Camera::state.n += 10;
}
    if (GetAsyncKeyState(VK_LEFT)){
        dYaw += turnSpeed; Camera::state.n += 10;
}
    if (GetAsyncKeyState(VK_RIGHT)){
        dYaw -= turnSpeed; Camera::state.n += 10;
}
    if (GetAsyncKeyState('E')){
        dRoll -= turnSpeed; Camera::state.n += 10;
}
    if (GetAsyncKeyState('Q')){
        dRoll += turnSpeed; Camera::state.n += 10;
}

        if (dPitch != 0.0f || dYaw != 0.0f || dRoll != 0.0f) {
            XMVECTOR qPitch = XMQuaternionRotationAxis(Hero::state.Right, dPitch);
            XMVECTOR qYaw = XMQuaternionRotationAxis(Hero::state.Up, dYaw);
            XMVECTOR qRoll = XMQuaternionRotationAxis(Hero::state.Forwardbuf, dRoll);

            XMVECTOR qTotal = XMQuaternionMultiply(qYaw, qPitch);
            qTotal = XMQuaternionMultiply(qTotal, qRoll);

            Hero::state.currentRotation = XMQuaternionMultiply(Hero::state.currentRotation, qTotal);
            Hero::state.currentRotation = XMQuaternionNormalize(Hero::state.currentRotation);

            // Обновляем базовые векторы
            Hero::state.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
            Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
            Hero::state.Right = XMVector3Cross(Hero::state.Up, Hero::state.Forwardbuf);
        }
        else Camera::state.n=0;
                    //убейте меня!!!!!!!!!!!

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
        Camera::state.constellationOffset = Camera::state.constellationOffset *
            XMMatrixTranslation(flyDirection.x * currentFlySpeed * deltaTime,
                flyDirection.y * currentFlySpeed * deltaTime,
                flyDirection.z * currentFlySpeed * deltaTime);

        //Camera::Camera(); // Обновляем камеру после перемещения (вот это дублирующее)
    }

}
