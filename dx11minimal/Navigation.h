point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

point3d flyDirection = { 0, 0, 0 };
point3d heroPosition = { 0, 0, 0 };
XMMATRIX constellationOffset= XMMatrixTranslation(0, 0, 0);
float currentFlySpeed = 0.0f;
const float maxFlySpeed = 0.1f;
const float flyAcceleration = 0.2f;
const float flyDeceleration = 0.002f;

void updateFlyDirection() { // Раскладка управления 
    flyDirection = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) flyDirection.z += .1f;// Вперёд
    if (GetAsyncKeyState('A') & 0x8000) flyDirection.x -= .1f;// Влево
    if (GetAsyncKeyState('S') & 0x8000) flyDirection.z -= .1f;// Назад
    if (GetAsyncKeyState('D') & 0x8000) flyDirection.x += .1f;// Вправо
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) flyDirection.y -= .1f;// Вверх
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) flyDirection.y += .1f;// Вниз


    float length = sqrt(flyDirection.x * flyDirection.x + flyDirection.y * flyDirection.y + flyDirection.z * flyDirection.z);

    if (length > 0) {
        flyDirection.x /= length;
        flyDirection.y /= length;
        flyDirection.z /= length;
    }
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

         constellationOffset = constellationOffset *XMMatrixTranslation(flyDirection.x * currentFlySpeed * deltaTime, flyDirection.y * currentFlySpeed * deltaTime, flyDirection.z * currentFlySpeed * deltaTime);
    }

}
