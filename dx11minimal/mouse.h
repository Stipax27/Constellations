void navigationByMouse();
void selectWeapon();

struct Mouse
{
    point3d pos;
    point3d Angle;
    point3d oldPos;
    point3d oldAngle;
    point3d worldPos;

    void Input()
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(window.hWnd, &p); // ���������� �����.

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

    // ���������� ��������� ��������� ����
    float dx = (mouse.pos.x - mouse.oldPos.x) * 0.01f;
    float dy = (mouse.pos.y - mouse.oldPos.y) * 0.01f;

    // ���������� �������� �� ������������ ��� (yaw)
    XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dx);

    // ���������� �������� �� �������������� ��� (pitch)
    XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), dy);

    XMVECTOR finalRotationQuat = XMQuaternionMultiply(qYaw, qPitch);

    finalRotationQuat = XMQuaternionNormalize(finalRotationQuat);

    rotationMatrix = XMMatrixRotationQuaternion(finalRotationQuat);

}
void navigationByMouse()
{
    static XMVECTOR currentRotation = XMQuaternionIdentity();
    float dYawKeyboard = 0.0f;
    if (GetAsyncKeyState('E')) { dYawKeyboard -= turnSpeed; }
    if (GetAsyncKeyState('Q')) { dYawKeyboard += turnSpeed; }
    XMVECTOR qYawTotal = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), dYawKeyboard);
    Camera::state.currentRotation = XMQuaternionMultiply(qYawTotal, Camera::state.currentRotation);
    Camera::state.currentRotation = XMQuaternionNormalize(Camera::state.currentRotation);
    Camera::state.Forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Camera::state.currentRotation);
    Camera::state.Up = XMVector3Rotate(Camera::state.defaultUp, Camera::state.currentRotation);
    Camera::state.Right = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), Camera::state.currentRotation);

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

        // ������������� dy ��� ��������������� ��� Y
        XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), -dy); // ��w������ ���� dy
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -dx);

        float dYawKeyboard = 0.0f;
        //if (GetAsyncKeyState(0x44)){ dYawKeyboard -= turnSpeed ; }
        //if (GetAsyncKeyState(0x41)){dYawKeyboard += turnSpeed ; }

        // ������������ ������ ����������� �������� ������ ��� Y
        XMVECTOR qYawTotal = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), dYawKeyboard);

        // ����� ���������� ��������
        XMVECTOR qNewRotation = XMQuaternionMultiply(qYawTotal, qPitch);
        qNewRotation = XMQuaternionMultiply(qYaw, qNewRotation);
        Camera::state.currentRotation = XMQuaternionMultiply(qNewRotation, Camera::state.currentRotation);
        Camera::state.currentRotation = XMQuaternionNormalize(Camera::state.currentRotation);
        Camera::state.Forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), Camera::state.currentRotation);
        Camera::state.Up = XMVector3Rotate(Camera::state.defaultUp, Camera::state.currentRotation);
        Camera::state.Right = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), Camera::state.currentRotation);

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
            currentColorIndex = min(numColors * (atan2(dy, -dx) / (2. * PI) + .5), numColors - 1);// ��� ������� ��� �������� ���� � ���� ������� �� �������� �� ����. �����.
        }
        else
        {
            currentColorIndex = -1;
        }
    }
}


point3d GetMouseRay(point3d pos)
{
    XMMATRIX view = XMMatrixLookAtLH(Camera::state.Eye, Camera::state.at, Camera::state.Up);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::state.fovAngle), iaspect, 0.01f, 10000.0f);

    float x = (2.0f * pos.x) / window.width - 1.0f;
    float y = 1.0f - (2.0f * pos.y) / window.height;

    XMVECTOR rayClip = XMVectorSet(x, y, 1.0f, 1.0f);

    XMVECTOR rayEye = XMVector4Transform(rayClip, XMMatrixInverse(nullptr, proj));
    rayEye = XMVectorSet(XMVectorGetX(rayEye), XMVectorGetY(rayEye), 1.0f, 0.0f);

    XMVECTOR ray = XMVector4Transform(rayEye, XMMatrixInverse(nullptr, view));
    point3d rayWorld = point3d(XMVectorGetX(ray), XMVectorGetY(ray), XMVectorGetZ(ray));
    rayWorld = rayWorld.normalized();

    return rayWorld;
}