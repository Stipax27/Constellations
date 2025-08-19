point3d flyDirection = point3d(0, 0, 1);
point3d flyRightDirection = point3d(1, 0, 0);
point3d flyUpDirection = point3d(0, 1, 0);

float currentFlySpeed = 0.0f;
const float maxFlySpeed = 10.f;
const float flyAcceleration = 5.f;
const float flyDeceleration = 0.002f;
const float boostFlySpeed = 15.f;
const float MOUSE_SENSITIVITY = 0.002f;

const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.3f;
const float SENSIVITY = 0.25f;
const float CURSOR_ZONE_DELTA = MAX_CURSOR_DEVIATION - CURSOR_IGNORE_ZONE;

int moveBlockTime = 400; // time for dash, and block movement
float localTime = -10000000000000; //localTime from updateFlyPosition
void updateFlyDirection() 
{ // ��������� ���������� 
    if (currentTime - localTime <= moveBlockTime) 
    {
        return;
    }
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
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {

        flyDirection.x += XMVectorGetX(Hero::state.Up) * 5.f;
        flyDirection.y += XMVectorGetY(Hero::state.Up) * 5.f;
        flyDirection.z += XMVectorGetZ(Hero::state.Up) * 5.f;
    }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    

        flyDirection.x -= XMVectorGetX(Hero::state.Up) * 5.f;
        flyDirection.y -= XMVectorGetY(Hero::state.Up) * 5.f;
        flyDirection.z -= XMVectorGetZ(Hero::state.Up) * 5.f;

    }

    if (flyDirection.magnitude() > 0)
    {
        flyDirection = flyDirection.normalized();
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

//int galo = 0;
//galo += 16;
//if (0 < galo < moveBlockTime - 300)
//    currentFlySpeed = lerp(targetSpeed, Hero::state.timeShiftPressed, galo / 100);
//else
//if (300 > galo > moveBlockTime)
//currentFlySpeed = lerp(Hero::state.timeShiftPressed, targetSpeed, (galo / 400 - 0.75) * 4);


float targetSpeed = 0;

void updateFlySpeed(float deltaTime) 
{// yskorenie

    bool isMoving = (flyDirection.x != 0 || flyDirection.y != 0 || flyDirection.z != 0);
    static bool wasShiftPressed = false;
    bool isBoosting = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
    targetSpeed = isBoosting ? boostFlySpeed : maxFlySpeed;

    if (!isBoosting)
    {
        if (isMoving && currentTime - localTime > moveBlockTime)
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
        Hero::state.timeShiftPressed += 1;
        wasShiftPressed = isBoosting;
        currentFlySpeed = 0;
    }
    if (!isBoosting && wasShiftPressed)
    {
        currentFlySpeed = Hero::state.timeShiftPressed;

        wasShiftPressed = false;
        localTime = currentTime;
        Hero::state.timeShiftPressed = 0;
        
    }
}



void updatePlayerPosition(float deltaTime) 
{
    if (currentFlySpeed > 0)
    {
        // 1. Создаем вектор направления из flyDirection
        XMVECTOR moveDir = XMVectorSet(flyDirection.x, flyDirection.y, flyDirection.z, 0.0f);

        // 2. Рассчитываем вектор смещения (без дополнительного вращения!)
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