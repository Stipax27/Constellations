point3d flyDirection = point3d(0, 0, 1);
point3d flyRightDirection = point3d(1, 0, 0);
point3d flyUpDirection = point3d(0, 1, 0);

float currentFlySpeed = 10.0f;
float boostingFlySpeed;
const float maxFlySpeed = 15.f;
const float MOUSE_SENSITIVITY = 0.002f;
const float CURSOR_IGNORE_ZONE = 0.05f;
const float MAX_CURSOR_DEVIATION = 0.3f;
const float SENSIVITY = 0.25f;
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
{ // ��������� ���������� 
    if (currentFlySpeed > maxFlySpeed) 
    {
        return;
    }
    point3d Direction = { 0, 0, 0 };

    if (GetAsyncKeyState('W') & 0x8000) {
        // ��������� ������ ����������� ������ (��� ������������)
        Direction.x += XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        Direction.y += XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        Direction.z += XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        // �������� ����� - �������� �����������
        Direction.x -= XMVectorGetX(Hero::state.Forwardbuf) * 10.0f;
        Direction.y -= XMVectorGetY(Hero::state.Forwardbuf) * 10.0f;
        Direction.z -= XMVectorGetZ(Hero::state.Forwardbuf) * 10.0f;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        // ��������� ������ ����������� ������ (��� ������������)
        Direction.x -= XMVectorGetX(Hero::state.Right) * 5.f;
        Direction.y -= XMVectorGetY(Hero::state.Right) * 5.f;
        Direction.z -= XMVectorGetZ(Hero::state.Right) * 5.f;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        // �������� ����� - �������� �����������
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

int acc = 0; 
bool wasShiftPressed = false;
float speed;
float localTime;
float localDeltaTime;

LONG energy = 100;
void updateFlySpeed(float deltaTime)
{ 
    //drawString("HI GUYS AMERICAAAAAA", 1000.f, 800.f, .7f, false);

    bool haveEnergy = true;

    bool isBoosting = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

    if (energy < 300 && currentFlySpeed < 20 && !isBoosting) {
        energy += LONG((deltaTime / 17.f ));
    }

    drawString((std::to_string(energy)).c_str(), 1000.f, 800.f, .7f, false);
    //drawString((std::to_string(deltaTime / 17.f)).c_str(), 1000.f, 800.f, .7f, false);


    if (energy <= 0.f) {
        //isBoosting = false;
        haveEnergy = false;
    }

    deltaTime /= 1000;
    speed = max(speed + acc * deltaTime, 0);

    if (haveEnergy) {

        //drawString("suja!!!", 1000.f, 560.f, .7f, false);


        if (!isBoosting && !wasShiftPressed)
        {
            //drawString("state1", 1000.f, 580.f, .7f, false);

            currentFlySpeed = max(currentFlySpeed + acc * deltaTime, maxFlySpeed);
        }

        if (isBoosting && !wasShiftPressed)
        {
            //drawString("state2", 1000.f, 600.f, .7f, false);

            speed = 0;
            currentFlySpeed = 10;
            wasShiftPressed = true;
            acc = 40; //тут должна быть переменная зависящая от фпс и времени
            localTime = currentTime;
        }

        if (isBoosting)
        {
            
            //drawString("state3", 1000.f, 620.f, .7f, false);

            energy -= 1.5;

            currentFlySpeed = max(currentFlySpeed * 0.9, 0);
        }

        if (!isBoosting && wasShiftPressed)
        {
            //drawString("state4", 1000.f, 640.f, .7f, false);

            localDeltaTime = currentTime - localTime;
            wasShiftPressed = false;
            boostingFlySpeed = 10; //тут должна быть переменная зависящая от времени зажатия шифта
            currentFlySpeed = boostingFlySpeed + speed;
            acc = -40;//тут должна быть переменная зависящая от фпс и времени

            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Whoosh.wav");
        }
    }
    //}
    //if (!isBoosting)
    //{
    //    //currentFlySpeed = min(currentFlySpeed * 1.5, boostFlySpeed);

    //    if (isMoving)
    //    {

    //        currentFlySpeed = min(currentFlySpeed*1.05, maxFlySpeed);

    //    }
    //    else
    //    {
    //        currentFlySpeed = max(currentFlySpeed*0.8, 0);
    //        
    //    }

    //    if (wasShiftPressed)
    //    {
    //        currentFlySpeed = Hero::state.timeShiftPressed;
    //        wasShiftPressed = false;
    //        localTime = currentTime;
    //        Hero::state.timeShiftPressed = 0;

    //    }
    //}
    //else
    //{
    //    Hero::state.timeShiftPressed += 3;
    //    wasShiftPressed = true;
    //    currentFlySpeed = 0;
    //}

    /*if (!isBoosting && wasShiftPressed)
    {
        currentFlySpeed = Hero::state.timeShiftPressed;
        wasShiftPressed = false;
        localTime = currentTime;
        Hero::state.timeShiftPressed = 0;
        
    }*/
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

