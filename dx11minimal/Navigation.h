point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };

point3d flyDirection = { 0, 0, 0 };
point3d heroPosition = { 0, 0, 0 };
float currentFlySpeed = 0.0f;
const float maxFlySpeed = 0.1f;
const float flyAcceleration = 0.2f;
const float flyDeceleration = 0.002f;
const float MOUSE_SENSITIVITY = 0.002f;

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

    
        // Получаем текущую позицию мыши
        POINT currentMousePos;
        GetCursorPos(&currentMousePos);
        ScreenToClient(window.hWnd, &currentMousePos);
        
        //if (!rmb) {
        //    // Первое движение после нажатия - запоминаем позицию
        //    rmb = true;
        //    mouse.oldPos.x = (float)currentMousePos.x;
        //    mouse.oldPos.y = (float)currentMousePos.y;
        //}
        //else {
        //    // Вычисляем разницу в движении мыши
        //    float dx = (currentMousePos.x - window.width/2) * 0.01f;
        //    float dy = (currentMousePos.y - window.height/2) * 0.01f;

        //    // Применяем к поворотам
        //    dPitch = dy; // Вертикальное движение мыши - pitch (инвертируем dy)
        //    dYaw = dx;   // Горизонтальное движение мыши - yaw

        //    // Обновляем старую позицию
        //    mouse.oldPos.x = (float)currentMousePos.x;
        //    mouse.oldPos.y = (float)currentMousePos.y;

        //    Camera::state.n += 10; // Аналогично тому, что было при нажатии клавиш
        //}

        //SetCursorPos(window.width / 2, window.height / 2);


        float x = currentMousePos.x - window.width / 2;
        float y = currentMousePos.y - window.height / 2;
        
        point3d mousePos = point3d(x / window.width, y / window.height);
        float length = mousePos.magnitude();

        if (length > 0.05f)
        {
            if (length > 0.2f)
            {
                mousePos = mousePos.normalized() * 0.2f;
                SetCursorPos(mousePos.x * window.width + window.width / 2, mousePos.y * window.height + window.height / 2);
            }

            float dx = (mousePos.x) * 0.5f;
            float dy = (mousePos.y) * 0.5f;

            // Применяем к поворотам
            dPitch = dy; // Вертикальное движение мыши - pitch (инвертируем dy)
            dYaw = dx;   // Горизонтальное движение мыши - yaw
        }
    

    // Обработка Q и E для крена (оставляем как было)
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

        // Обновляем базовые векторы
        Hero::state.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Hero::state.currentRotation);
        Hero::state.Up = XMVector3Rotate(Hero::state.defaultUp, Hero::state.currentRotation);
        Hero::state.Right = XMVector3Cross(Hero::state.Up, Hero::state.Forwardbuf);
    }
    else {
        Camera::state.n = lerp(Camera::state.n, 0, 0.2f);
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
        Camera::state.constellationOffset = Camera::state.constellationOffset *
            XMMatrixTranslation(flyDirection.x * currentFlySpeed * deltaTime,
                flyDirection.y * currentFlySpeed * deltaTime,
                flyDirection.z * currentFlySpeed * deltaTime);

        //Camera::Camera(); // Обновляем камеру после перемещения (вот это дублирующее)
    }

}
