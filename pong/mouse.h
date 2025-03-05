POINT mouse;
point3d mouseAngle;
point3d oldmouse;
point3d oldmouseAngle;
bool rmb = false; //������ ������ ����
bool lmb = false;

void navigationByMouse()
{
    if (GetAsyncKeyState(VK_RBUTTON))
    {
        if (!rmb)
        {
            rmb = true;
            oldmouse.x = mouse.x;
            oldmouse.y = mouse.y;
            oldmouseAngle = mouseAngle;
        }
        float dx, dy;
        dx = mouse.x - oldmouse.x;
        dy = mouse.y - oldmouse.y;

        mouseAngle.x = oldmouseAngle.x + dx;
        mouseAngle.y = oldmouseAngle.y + dy;
    }
    else
    {
        rmb = false;
    }// ��� ��������� ��� �������� ������������.
}

void selectWeapon()
{
    if (GetAsyncKeyState(VK_LBUTTON))
    {
        float dx = mouse.x - window.width / 2.;
        float dy = mouse.y - window.height / 2.;
        float lenght = sqrt(dx * dx + dy * dy);
       
        if (lenght < circleRadius)
        {
            currentColorIndex = min(numColors * (atan2(dy, -dx) / (2. * PI) + .5), numColors - 1);// ��� ������� ��� �������� ���� � ���� ������� �� �������� �� ����. �����.
        }
        else
        {
            currentColorIndex = -1;
        }
    }
}

void mouseInput()
{
    GetCursorPos(&mouse);
    ScreenToClient(window.hWnd, &mouse); // ���������� �����.
    navigationByMouse();
    selectWeapon();
}