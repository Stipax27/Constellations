//ot mouse teleport sozvesdia ne zavisit
point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

point3d flyDirection = { 0, 0, 0 };
//point3d heroPosition = { 0, 0, 0 };
float currentFlySpeed = 0.0f;
const float maxFlySpeed = 10.f;
const float flyAcceleration = 5.f;
const float flyDeceleration = 0.002f;
const float boostFlySpeed = 9.f;
const float MOUSE_SENSITIVITY = 0.002f;

const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.3f;
const float SENSIVITY = 0.25f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;


void updateFlyDirection() 
{ // ��������� ���������� 
    flyDirection = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) {
        // ��������� ������ ����������� ������ (��� ������������)
        flyDirection.x += XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.y += XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.z += XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        // �������� ����� - �������� �����������
        flyDirection.x -= XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.y -= XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        // ��������� ������ ����������� ������ (��� ������������)
        flyDirection.x -= XMVectorGetX(Hero::state.Right) * 5.f;
        flyDirection.y -= XMVectorGetY(Hero::state.Right) * 5.f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Right) * 5.f;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        // �������� ����� - �������� �����������
        flyDirection.x += XMVectorGetX(Hero::state.Right) * 5.f;
        flyDirection.y += XMVectorGetY(Hero::state.Right) * 5.f;
        flyDirection.z += XMVectorGetZ(Hero::state.Right) * 5.f;
    }

    float dPitch = 0.0f, dYaw = 0.0f, dRoll = 0.0f;


    float x = mouse.pos.x - window.width / 2;
    float y = mouse.pos.y - window.height / 2;

    point3d mousePos = point3d(x / window.width / aspect, y / window.height,0);
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
        //
        // ��������� � ���������
        dPitch = dy; // ������������ �������� ���� - pitch (����������� dy)
        dYaw = dx;   // �������������� �������� ���� - yaw
    }


    // ��������� Q � E ��� ����� (��������� ��� ����)
    if (GetAsyncKeyState('E')) {
        dRoll -= turnSpeed;
        Camera::state.n = lerp(Camera::state.n, 100, 0.3f);
    }
    if (GetAsyncKeyState('Q')) {
        dRoll += turnSpeed;
        Camera::state.n = lerp(Camera::state.n, 100, 0.3f);
    }

    if (dPitch != 0.0f || dYaw != 0.0f || dRoll != 0.0f) {
        XMVECTOR qPitch = XMQuaternionRotationAxis(Hero::state.Right, dPitch);
        XMVECTOR qYaw = XMQuaternionRotationAxis(Hero::state.Up, dYaw);
        XMVECTOR qRoll = XMQuaternionRotationAxis(Hero::state.Forwardbuf, dRoll);

        XMVECTOR qTotal = XMQuaternionMultiply(qYaw, qPitch);
        qTotal = XMQuaternionMultiply(qTotal, qRoll);

        Hero::state.currentRotation = XMQuaternionMultiply(Hero::state.currentRotation, qTotal);
        Hero::state.currentRotation = XMQuaternionNormalize(Hero::state.currentRotation);

        // ��������� ������� �������
        Hero::state.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
        Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
        Hero::state.Right = XMVector3Cross(Hero::state.Up, Hero::state.Forwardbuf);
    }
    else {
        Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
    }
    
}

void updateFlySpeed(float deltaTime) {// yskorenie
    bool isMoving = (flyDirection.x != 0 || flyDirection.y != 0 || flyDirection.z != 0);

    bool isBoosting = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
    float targetSpeed = 0.2f; // isBoosting ? boostFlySpeed : maxFlySpeed;

    if (!isBoosting)
    {
        if (isMoving)
        {
            currentFlySpeed += flyAcceleration * deltaTime;

            if (currentFlySpeed > targetSpeed)
            {
                currentFlySpeed = targetSpeed;
            }
        }
        else
        {
            currentFlySpeed -= flyDeceleration * deltaTime;
            if (currentFlySpeed < 0)
            {
                currentFlySpeed = 0;
            }
        }
    }
    else
    {

    }
}

void updatePlayerPosition(float deltaTime) 
{
    if (currentFlySpeed > 0)
    {
        // 1. Создаем вектор направления из flyDirection
        XMVECTOR moveDir = XMVectorSet(flyDirection.x, flyDirection.y, flyDirection.z, 0.0f);

        // 2. Нормализуем вектор направления (если он не нулевой)
        float length = XMVectorGetX(XMVector3Length(moveDir));
        if (length > 0.001f)
        {
            moveDir = XMVector3Normalize(moveDir);

            // 3. Рассчитываем вектор смещения (без дополнительного вращения!)
            XMVECTOR displacement = XMVectorScale(moveDir, currentFlySpeed * deltaTime);

            // 4. Обновляем позицию героя
            Hero::state.position = XMVectorAdd(Hero::state.position, displacement);
        }

        // 5. Обновляем матрицу мира
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(Hero::state.currentRotation);
        Hero::state.worldMatrix = rotationMatrix * XMMatrixTranslationFromVector(Hero::state.position);

        // 6. Обновляем constellationOffset
        Hero::state.constellationOffset = XMMatrixTranslationFromVector(Hero::state.position);

        // 7. Нормализуем векторы ориентации
        Hero::state.Forwardbuf = XMVector3Normalize(Hero::state.Forwardbuf);
        Hero::state.Up = XMVector3Normalize(Hero::state.Up);
        Hero::state.Right = XMVector3Normalize(Hero::state.Right);
    }
}