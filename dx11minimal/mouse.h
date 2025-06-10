
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
//Mouse::mouse;
//POINT& mouse, point3d& mouseAngle, point3d& oldmouse, point3d& oldmouseAngle
void navigationByMouse()
{
    if (GetAsyncKeyState(VK_RBUTTON))
    {
        if (!rmb)
        {
            rmb = true;
            mouse.oldPos.x = mouse.pos.x;
            mouse.oldPos.y = mouse.pos.y;
            mouse.oldAngle = mouse.Angle;
        }
        float dx, dy;
        dx = mouse.pos.x - mouse.oldPos.x;
        dy = mouse.pos.y - mouse.oldPos.y;

        auto rm = XMMatrixRotationRollPitchYaw(dy * 0.01, dx * 0.01, 0);

       
        ConstBuf::camera.view[0]=XMMatrixMultiply(rm, ConstBuf::camera.view[1]);
        ConstBuf::UpdateCamera();
        mouse.Angle.x = mouse.oldAngle.x + dx;
        mouse.Angle.y = mouse.oldAngle.y + dy;

    }
    else
    {
        ConstBuf::camera.view[1] = ConstBuf::camera.view[0];
        rmb = false;
    }// При удержании ПКМ кручение простарнства.
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


