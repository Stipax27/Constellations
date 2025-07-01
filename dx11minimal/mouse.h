
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

        float dx = (mouse.pos.x - mouse.oldPos.x)*0.01 ;
        float dy = (mouse.pos.y - mouse.oldPos.y)*0.01;

        XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), dy);
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dx);

        XMVECTOR qNewRotation = XMQuaternionMultiply(qYaw, qPitch);
        Camera::state.currentRotation = XMQuaternionMultiply(qNewRotation, Camera::state.currentRotation);
        Camera::state.currentRotation = XMQuaternionNormalize(Camera::state.currentRotation);

        XMVECTOR rotatedForward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Camera::state.currentRotation);
        XMVECTOR rotatedUp = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), Camera::state.currentRotation);
        XMVECTOR eye = XMVectorScale(rotatedForward, -Camera::state.camDist);

        ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(eye, Camera::state.at, rotatedUp));

        ConstBuf::UpdateCamera();
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


