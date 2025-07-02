
void navigationByMouse();
void selectWeapon();

struct Mouse
{
    point3d pos;
    point3d Angle;
    point3d oldPos;
    point3d oldAngle;

    void Input()
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(window.hWnd, &p); // Управление мышью.

        pos = { (float)p.x, (float)p.y, 0.f };

        navigationByMouse();
        selectWeapon();
    }

};

Mouse mouse;
bool keyA = false;
bool keyD = false;
bool rmb = false;
bool lmb = false;

const float turnSpeed = 0.05f;

void rotationMatrix(XMMATRIX& rotationMatrix)
{
    if (!rmb)
    {
        rmb = true;
        mouse.oldPos.x = mouse.pos.x;
        mouse.oldPos.y = mouse.pos.y;
        mouse.oldAngle = mouse.Angle;
    }

    // Определяем изменения координат мыши
    float dx = (mouse.pos.x - mouse.oldPos.x) * 0.01f;
    float dy = (mouse.pos.y - mouse.oldPos.y) * 0.01f;

    // Кватернион вращения по вертикальной оси (yaw)
    XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dx);

    // Кватернион вращения по горизонтальной оси (pitch)
    XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), dy);

    XMVECTOR finalRotationQuat = XMQuaternionMultiply(qYaw, qPitch);

    finalRotationQuat = XMQuaternionNormalize(finalRotationQuat);

    rotationMatrix = XMMatrixRotationQuaternion(finalRotationQuat);

}
void navigationByMouse()
{
    static XMVECTOR currentRotation = XMQuaternionIdentity();
    float dYawKeyboard = 0.0f;
    if (GetAsyncKeyState(0x44)) { dYawKeyboard -= turnSpeed; }
    if (GetAsyncKeyState(0x41)) { dYawKeyboard += turnSpeed; }
    XMVECTOR qYawTotal = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), dYawKeyboard);
    Camera::state.currentRotation = XMQuaternionMultiply(qYawTotal, Camera::state.currentRotation);
    Camera::state.currentRotation = XMQuaternionNormalize(Camera::state.currentRotation);
    Camera::state.Forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Camera::state.currentRotation);
    Camera::state.Up = XMVector3Rotate(Camera::state.defaultUp, Camera::state.currentRotation);

    Camera::state.Eye = Camera::state.at - XMVectorScale(Camera::state.Forward, Camera::state.camDist);
    Camera::Camera();
    if (Camera::state.mouse)
    if (GetAsyncKeyState(VK_RBUTTON))
    {
        if (!rmb)
        {
            rmb = true;
            mouse.oldPos.x = mouse.pos.x;
            mouse.oldPos.y = mouse.pos.y;
            mouse.oldAngle = mouse.Angle;
        }
        float x = Camera::state.constellationOffset.r[3].m128_f32[0];
        float y = Camera::state.constellationOffset.r[3].m128_f32[1];
        float z = Camera::state.constellationOffset.r[3].m128_f32[2];
        Camera::state.at = XMVectorSet(x, y, z, 0)*20;
        float dx = (mouse.pos.x - mouse.oldPos.x) * 0.01;
        float dy = (mouse.pos.y - mouse.oldPos.y) * 0.01;

        // Инвертировали dy для инвертированной оси Y
        XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), -dy); // Изwменили знак dy
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -dx);

        float dYawKeyboard = 0.0f;
        if (GetAsyncKeyState(0x44)){ dYawKeyboard -= turnSpeed ; }
        if (GetAsyncKeyState(0x41)){dYawKeyboard += turnSpeed ; }

        // Формирование общего кватерниона поворота вокруг оси Y
        XMVECTOR qYawTotal = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), dYawKeyboard);

        // Общий кватернион поворота
        XMVECTOR qNewRotation = XMQuaternionMultiply(qYawTotal, qPitch);
        qNewRotation = XMQuaternionMultiply(qYaw, qNewRotation);
        Camera::state.currentRotation = XMQuaternionMultiply(qNewRotation, Camera::state.currentRotation);
        Camera::state.currentRotation = XMQuaternionNormalize(Camera::state.currentRotation);
        Camera::state.Forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Camera::state.currentRotation);
        Camera::state.Up = XMVector3Rotate(Camera::state.defaultUp, Camera::state.currentRotation);

        Camera::state.Eye = Camera::state.at - XMVectorScale(Camera::state.Forward, Camera::state.camDist);
        Camera::Camera();

        mouse.Angle.x = mouse.oldAngle.x + dx;
        mouse.Angle.y = mouse.oldAngle.y + dy;
        mouse.oldPos.x = mouse.pos.x;
        mouse.oldPos.y = mouse.pos.y;
    }
    else
    {
        rmb = false;
    }
}

void selectWeapon()
{
    if (GetAsyncKeyState(VK_LBUTTON))
    {
        float dx = mouse.pos.x - window.width / 2.;
        float dy = mouse.pos.y - window.height / 2.;
        float lenght = sqrt(dx * dx + dy * dy);
        if (lenght < circleRadius)
        {
            currentColorIndex = min(numColors * (atan2(dy, -dx) / (2. * PI) + .5), numColors - 1);// При нажатии ЛКМ Перекрас мыши в цвет который мы выбррали на Цвет. круге.
        }
        else
        {
            currentColorIndex = -1;
        }
    }
}


