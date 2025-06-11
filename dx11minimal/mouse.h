
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

bool rmb = false;
bool lmb = false;

void navigationByMouse()
{
    static XMVECTOR currentRotation = XMQuaternionIdentity();

    if (GetAsyncKeyState(VK_RBUTTON))
    {
        if (!rmb)
        {
            rmb = true;
            mouse.oldPos.x = mouse.pos.x;
            mouse.oldPos.y = mouse.pos.y;
            mouse.oldAngle = mouse.Angle;
        }

        float dx = (mouse.pos.x - mouse.oldPos.x) ;
        float dy = (mouse.pos.y - mouse.oldPos.y);

        // Создаем кватернионы для вращений вокруг осей Y (yaw) и X (pitch)
        XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), dy);
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dx);

        // Комбинируем вращения
        XMVECTOR qNewRotation = XMQuaternionMultiply(qPitch, qYaw);

        // Обновляем текущий кватернион вращения
        currentRotation = XMQuaternionMultiply(qNewRotation, currentRotation);
        currentRotation = XMQuaternionNormalize(currentRotation);

        // Преобразуем кватернион в матрицу вида
        ConstBuf::camera.view[0] = XMMatrixRotationQuaternion(currentRotation);

        ConstBuf::UpdateCamera();
        mouse.Angle.x = mouse.oldAngle.x + dx;
        mouse.Angle.y = mouse.oldAngle.y + dy;
        mouse.oldPos.x = mouse.pos.x;
        mouse.oldPos.y = mouse.pos.y;
    }
    else
    {
        ConstBuf::camera.view[1] = ConstBuf::camera.view[0];
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


