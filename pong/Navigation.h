point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

point3d flyDirection = { 0, 0, 0 };
point3d heroPosition = { 0, 0, 0 };

float currentFlySpeed = 0.0f;
const float maxFlySpeed = 0.5f;
const float flyAcceleration = 0.001f;
const float flyDeceleration = 0.002f;

void updateFlyDirection() {
    flyDirection = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) flyDirection.z += 1.0f;
    if (GetAsyncKeyState('A') & 0x8000) flyDirection.x -= 1.0f;
    if (GetAsyncKeyState('S') & 0x8000) flyDirection.z -= 1.0f;
    if (GetAsyncKeyState('D') & 0x8000) flyDirection.x += 1.0f;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) flyDirection.y -= 1.0f;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) flyDirection.y += 1.0f;

    // ������������ ������� �����������
    float length = sqrt(flyDirection.x * flyDirection.x +
        flyDirection.y * flyDirection.y +
        flyDirection.z * flyDirection.z);
    if (length > 0) {
        flyDirection.x /= length;
        flyDirection.y /= length;
        flyDirection.z /= length;
    }
}

void updateFlySpeed(float deltaTime) {
    bool isMoving = (flyDirection.x != 0 || flyDirection.y != 0 || flyDirection.z != 0);

    if (isMoving) {
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

void updatePlayerPosition(float deltaTime, point3d& p) {
    if (currentFlySpeed > 0) {
        // ��������� ���������� ������� �����
        heroPosition.x += flyDirection.x * currentFlySpeed * deltaTime;
        heroPosition.y += flyDirection.y * currentFlySpeed * deltaTime;
        heroPosition.z += flyDirection.z * currentFlySpeed * deltaTime;
       
        // ��������� ��������� � ���������� �����
        p.x = heroPosition.x;
        p.y = heroPosition.y;
        p.z = heroPosition.z;
    }
}
