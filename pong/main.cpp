//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#define _CRT_SECURE_NO_WARNINGS

const float PI = 3.1415926535897;

#include "windows.h"
#include "vector"
#include <stdexcept>
#include "math.h"
#include <stdlib.h>

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ������ ������ ����  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//����� � ������� ������ 
} sprite;
float starSize = 10;

sprite racket;//������� ������
sprite enemy;//������� ����������
sprite ball;//�����

int startTime;

struct {
    int score, balls;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
} game;

struct {
    HWND hWnd;//����� ����
    HDC device_context, context;// ��� ��������� ���������� (��� �����������)
    int width, height;//���� �������� ������� ���� ������� ������� ���������
} window;   

HBITMAP hBack;// ����� ��� �������� �����������

typedef struct {
    float x;
    float y;
    float z;
} point3d;

void rotateworld();
void rotateworld(point3d& p);
//c����� ����

void InitGame()
{
    //� ���� ������ ��������� ������� � ������� ������� gdi
    //���� ������������� - ����� ������ ������ ����� � .exe 
    //��������� ������ LoadImageA ��������� � ������� ��������, ��������� �������� ����� ������������� � ������� ���� �������
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

    racket.width = 300;
    racket.height = 50;
    racket.speed = 30;//�������� ����������� �������
    racket.x = window.width / 2.;//������� ���������� ����
    racket.y = window.height - racket.height;//���� ���� ���� ������ - �� ������ �������

    enemy.x = racket.x;//� ���������� �������� ������ � �� �� ����� ��� � ������

    ball.dy = (rand() % 65 + 35) / 100.;//��������� ������ ������ ������
    ball.dx = -(1 - ball.dy);//��������� ������ ������ ������
    ball.speed = 11;
    ball.rad = 20;
    ball.x = racket.x;//x ���������� ������ - �� ������� �������
    ball.y = racket.y - ball.rad;//����� ����� ������ �������

    game.score = 0;
    game.balls = 9;

    startTime = timeGetTime();
   
}

void ProcessSound(const char* name)//������������ ���������� � ������� .wav, ���� ������ ������ � ��� �� ����� ��� � ���������
{
    PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//���������� name �������� ��� �����. ���� ASYNC ��������� ����������� ���� ���������� � ����������� ���������
}

void ShowScore()
{
    //�������� �������� � �������
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//����� ��� ������
    _itoa_s(game.score, txt, 10);//�������������� �������� ���������� � �����. ����� �������� � ���������� txt
    TextOutA(window.context, 10, 10, "Score", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "Balls", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) racket.x -= racket.speed;
    if (GetAsyncKeyState(VK_RIGHT)) racket.x += racket.speed;

    if (!game.action && GetAsyncKeyState(VK_SPACE))
    {
        game.action = true;
        ProcessSound("bounce.wav");
    }
}

void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(hDC); // ������� �������� ������, ����������� � ���������� �����������
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// �������� ����������� bitmap � �������� ������

    if (hOldbm) // ���� �� ���� ������, ���������� ������
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // ���������� ������� �����������

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//��� ������� ������� ����� ����� ��������������� ��� ����������
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // ������ ����������� bitmap
        }

        SelectObject(hMemDC, hOldbm);// ��������������� �������� ������
    }

    DeleteDC(hMemDC); // ������� �������� ������
}


float getX(float x)
{
    return x * window.width;
}

float getY(float y)
{
    return y * window.height;
}

float saturate(float x)
{
    return max(min(x, 1.), 0.);// ������� ������.
}



point3d point;

void move(point3d& p, float x, float y, float z)
{
    p.x += x;
    p.y += y;
    p.z += z;
}

void rotateX(point3d& p, float angle)// ������� �� ��� X.
{
    float a = angle * PI / 180.;

    float x1 = p.x;
    float y1 = p.y * cos(a) - p.z * sin(a);
    float z1 = p.y * sin(a) + p.z * cos(a);

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

void rotateY(point3d &p, float angle)// ������� �� ��� Y.
{
    float a = angle * PI / 180.;

    float x1 = p.x * cos(a) - p.z * sin(a);
    float y1 = p.y;
    float z1 = p.x * sin(a) + p.z * cos(a);

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

void rotateZ(point3d& p, float angle)// ������� �� ��� Z.
{
    float a = angle * PI / 180.;

    float x1 = p.x * cos(a) - p.y * sin(a);
    float y1 = p.x * sin(a) + p.y * cos(a);
    float z1 = p.z;

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

float clamp(float x, float a, float b)
{
    return fmax(fmin(x, b), a);
}

float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}
float camDist = 0;

void project(point3d& p)
{
    int currentTime = timeGetTime() - startTime;
    float startCamDist = 100;
    float finCamDist = 3000;

    float lerpCoef = smoothstep(0.,1, min(currentTime*4, finCamDist)/(finCamDist- startCamDist));
    if (lerpCoef<.99) {
        camDist = lerpCoef * (finCamDist - startCamDist) + startCamDist;
    }
    camDist = clamp(camDist, 100, 3000);
    float x = window.width / 2. + point.x * camDist / (point.z + camDist);    
    float y = window.height / 2. + point.y * camDist / (point.z + camDist);   
                                                                              
    p.x = x;                                                                  
    p.y = y;                                                                  
                                                                              
}                                                                             
                                                                              
                                                                              
void genRandSphere(point3d& p)                                                
{
    float amp = 1.25;
    float angleX, angleY;
    angleX = rand() % 360;
    angleY = rand() % 360;
    p.x = 0;
    p.y = 0;
    p.z = window.width;
    rotateX(p, angleX);
    rotateY(p, angleY); // ������������� ������������.
}

void genRandCube(point3d& p)
{
    float amp = 1.25;
    p.x = (rand() % window.width - window.width / 2) * amp;
    p.y = (rand() % window.width - window.width / 2) * amp;
    p.z = (rand() % window.width - window.width / 2) * amp; // ����������� ����������� ������������.
}

void drawPoint(point3d& p, float sz = 2)
{
    if (p.z < 0) return;

    Ellipse(window.context,
        p.x - sz,
        p.y - sz,
        p.x + sz,
        p.y + sz
    );// ��������� ������.
    
}


POINT mouse;
point3d mouseAngle; 
point3d oldmouse;
point3d oldmouseAngle;
bool rmb = false; //������ ������ ����
bool lmb = false;

void drawLine(point3d& p1, point3d& p2, int count)
{
    for (int i = 0;i < count;i++)
    {
        
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dz = p2.z - p1.z;
        float length = sqrt(dx * dx + dy * dy + dz * dz);
        float stepX = dx / (float)count;
        float stepY = dy / (float)count;
        float stepZ = dz / (float)count;

        
        point.x = p1.x + stepX * (float)i;
        point.y = p1.y + stepY * (float)i;
        point.z = p1.z + stepZ * (float)i;

        project(point);

        float sz = 1+ .5*sinf(i + timeGetTime() * .01);
        drawPoint(point,sz);
        
        // ��������� �����.
    }
}

void show�onstellation(std::vector <float>& starArray, std::vector <float>& starHealth)
{
    int starsCount = starArray.size() / 3;

    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
    HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
    HBRUSH brush2 = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(window.context, pen);
    SelectObject(window.context, brush);// ����� � ����� ��� ��������� � �������������.
    
    for (int i = 0; i < starsCount-1; i++)
    {
        
        point3d point1, point2;
        point1.x = starArray[i * 3];
        point1.y = starArray[i * 3 + 1];
        point1.z = starArray[i * 3 + 2];

        point2.x = starArray[i * 3 + 3];
        point2.y = starArray[i * 3 + 3 + 1];
        point2.z = starArray[i * 3 + 3 + 2];

        float a = timeGetTime() * .01;
        //rotateworld(point1);
       // rotateworld(point2);
        
        if (starHealth[i] > 0 && starHealth[i + 1] > 0)
        {
            drawLine(point1, point2, 20);// ��������� ������� ����� ���������.
        }
        
    }

    for (int i = 0; i < starsCount; i++)
    {
        point.x = starArray[i * 3];
        point.y = starArray[i * 3 + 1];
        point.z = starArray[i * 3 + 2];

        float a = timeGetTime() * .01;
        //rotateworld();
        project(point);

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

        SelectObject(window.context, brush);// ������������� ���� ��� ��������� ����.

        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (lenght < starSize)
            {
                SelectObject(window.context, brush2);
                starHealth[i] -= .1;
            }
        }

        float sz = starSize*starHealth[i] + rad * 15;
        if (starHealth[i] > 0) //����� �� ���������� ��������� �����. 
        {
            drawPoint(point, sz);
        }

    }


    DeleteObject(pen);
    DeleteObject(brush);
    DeleteObject(brush2);
}



void arrange�onstellation(std::vector <float>& starArray, float angleX, float angleY, float angleZ)
{

    int starsCount = starArray.size() / 3;
    float scale = 1000;
    for (int i = 0; i < starsCount; i++)// ���������� �����.
    {
        

        point3d p = { starArray[i * 3 + 0], starArray[i * 3 + 1], starArray[i * 3 + 2] };

        move(p, 0, 0, 3000. / scale);
        rotateX(p, angleX);
        rotateY(p, angleY);
        rotateZ(p, angleZ);

        starArray[i * 3 + 0] = p.x * scale;
        starArray[i * 3 + 1] = p.y * scale;
        starArray[i * 3 + 2] = p.z * scale;
        
    }
}
void arrangePlayerConstallation(std::vector <float>& starArray, float angleX, float angleY, float angleZ) {
    int starsCount = starArray.size() / 3;
    float scale = 1000;
    for (int i = 0; i < starsCount; i++)// ���������� �����.
    {


        point3d p = { starArray[i * 3 + 0], starArray[i * 3 + 1], starArray[i * 3 + 2] };

        move(p, 0, 0, 3000. / scale);
        rotateX(p, angleX);
        rotateY(p, angleY);
        rotateZ(p, angleZ);

        starArray[i * 3 + 0] = p.x * scale;
        starArray[i * 3 + 1] = p.y * scale;
        starArray[i * 3 + 2] = p.z * scale;

    }
}

void showStarField()
{
    srand(10);
    for (int i = 0; i < 300; i++) {

        genRandSphere(point);
        rotateworld();
        project(point);
        drawPoint(point);
        // ����� �� ���� �� ���-��. � ���� ���������.
    }
}

float circleRadius;
float centerX;
float centerY;
const COLORREF colors[] = {
    RGB(255, 0, 0),    // �������
    RGB(255, 165, 0),  // ���������
    RGB(255, 255, 0),  // ������
    RGB(0, 255, 0),    // �������
    RGB(0, 255, 255),  // �������
    RGB(0, 0, 255),    // �����
    RGB(128, 0, 128)   // ����������
};

HBRUSH colorBrush[7];
int currentColorIndex = -1;

void drawColorCircle() {

    circleRadius = 75;
    centerX = window.width / 4;
    centerY = window.height / 4;

    int numColors = sizeof(colors) / sizeof(colors[0]);
    float angleStep = 2 * PI / (float)numColors;

    for (int i = 0; i < numColors; ++i) {

        float angle1 = -i * angleStep;
        float angle2 = -(i + 1) * angleStep;

        POINT p1, p2;
        p1.x = centerX + circleRadius * cos(angle1);
        p1.y = centerY + circleRadius * sin(angle1);

        p2.x = centerX + circleRadius * cos(angle2);
        p2.y = centerY + circleRadius * sin(angle2);


        HBRUSH brush = CreateSolidBrush(colors[i]);
        SelectObject(window.context, brush);
                
        drawPoint(point);

        Pie(window.context,
            centerX - circleRadius,
            centerY - circleRadius,
            centerX + circleRadius,
            centerY + circleRadius,
            p1.x, p1.y,
            p2.x, p2.y);

        DeleteObject(brush);// ������ ��� ��������� ����. �����.
    }

}

class starData {

    std::vector <float> x;
    std::vector <float> y;
    std::vector <float> z;
    std::vector <float> health;

    void Draw()
    {
        
    }
};

starData Aries_;

std::vector <float> Aries = { 0, 0, 0., .21, .05, 0., .35, .12, 0., .43, .27, 0. };
std::vector <float> Aries_health = { 1,1,1,1 };
std::vector <float> UrsaMajor = { -.25, -.25, 0., -.1, -.2, 0.,-.05, .05, 0.,.25, .05,0., .35, .25,0., .05, .25,0., -.05, .05,0. };
std::vector <float> UrsaMajor_health = { 1,1,1,1,1,1,1 };
std::vector <float> PieceOfCancer1 = { 0, 0, 0, .04, .25, 0, .08, .7, 0 };
std::vector <float> PieceOfCancer1_health = { 1,1,1 };
std::vector <float> Taurus = {0,0,0, .08, .7, .25, .04 };
std::vector <float> Taurus_health = {1,1};
std::vector <float> Gemini = {};
std::vector <float> Gemini_health = {};
std::vector <float> Leo = {};
std::vector <float> Leo_health = {};                                            // ������ ��� �������� (�� ����������).TEST
bool starsInitFlag = false;

void ShowRacketAndBall()
{

    GetCursorPos(&mouse);
    ScreenToClient(window.hWnd, &mouse); // ���������� �����.


    if (GetAsyncKeyState(VK_RBUTTON)) {

        if (!rmb) {
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
    else { rmb = false; }// ��� ��������� ��� �������� ������������.

    RECT rect;
    GetClientRect(window.hWnd, &rect);
    auto blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(window.context, &rect, blackBrush);
    DeleteObject(blackBrush);

    showStarField();

    if (!starsInitFlag)
    {
        arrange�onstellation(Aries, 17, -50, 0);
        arrange�onstellation(UrsaMajor, -15, -60, 0);
        arrange�onstellation(PieceOfCancer1, -20, -20, 0);
        arrange�onstellation(Taurus, 20, -30, 0);
        arrangePlayerConstallation(Taurus, -30,80, 0);
        starsInitFlag = true; // ���������� ���������� ��������� � ������������.
    }

    show�onstellation(Aries, Aries_health);
    show�onstellation(UrsaMajor, UrsaMajor_health);
    show�onstellation(PieceOfCancer1, PieceOfCancer1_health);
    show�onstellation(Taurus, Taurus_health);
    
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
    HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
    SelectObject(window.context, pen);
    SelectObject(window.context, brush);

    drawColorCircle();

    for (int i = 0; i < 7;i++)
    {
        colorBrush[i] = CreateSolidBrush(colors[i]);
        
    }

    if (GetAsyncKeyState(VK_LBUTTON)) {

        float dx = mouse.x - centerX;
        float dy = mouse.y - centerY;
        float lenght = sqrt(dx * dx + dy * dy);
        if (lenght < circleRadius) {
            
            currentColorIndex = min(7.*(atan2(dy, -dx) / (2. * PI) + .5),6);// ��� ������� ��� �������� ���� � ���� ������� �� �������� �� ����. �����.

            /*float s = atan2(dy, -dx) / (2. * PI) + .5;
            char sss[100];
            _itoa(s * 100, sss, 10);
            OutputDebugString(sss);
            OutputDebugString("\n");*/
        }
        else
        {
            currentColorIndex=-1;
        }


    }

    if (currentColorIndex == -1)
    {
        SelectObject(window.context, brush);
        
    }
    else
    {
        SelectObject(window.context, colorBrush[currentColorIndex]);
    }

    Ellipse(window.context,
        mouse.x - starSize ,
        mouse.y - starSize ,
        mouse.x + starSize ,
        mouse.y + starSize 
    );
    
    DeleteObject(pen);
    DeleteObject(brush);

    for (int i = 0; i < 7;i++)
    {
        DeleteObject(colorBrush[i]);
    }

    
}




void rotateworld()
{
    rotateX(point, mouseAngle.y * 0.1);
    rotateY(point, mouseAngle.x * 0.1);
    rotateZ(point, timeGetTime() * 0.01);
}

void rotateworld(point3d& p)
{
    rotateX(p, mouseAngle.y * 0.1);
    rotateY(p, mouseAngle.x * 0.1);
     rotateZ(point, timeGetTime() * 0.01); // �������� ��������.
}



void InitWindow()
{
    SetProcessDPIAware();
    //window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);


    hInst = (HINSTANCE)GetModuleHandle(0);
    auto width = GetSystemMetrics(SM_CXSCREEN);
    auto height = GetSystemMetrics(SM_CYSCREEN);
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
    RegisterClassEx(&wcex);
    window.hWnd = CreateWindow("fx", "fx", WS_OVERLAPPEDWINDOW, 0, 0, width, height, NULL, NULL, hInst, NULL);
    UpdateWindow(window.hWnd);
    ShowWindow(window.hWnd, SW_SHOW);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//�� ������ ���� ������� ����� ��������� ���������� ��� ���������
    window.width = r.right - r.left;//���������� ������� � ���������
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//������ �����
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//����������� ���� � ���������
    GetClientRect(window.hWnd, &r);

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
    switch (message)
    {
    case WM_MOUSEWHEEL:
    {    float zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        camDist += zDelta;
        break;
    }
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����
    InitGame();//����� �������������� ���������� ����

    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    
    MSG msg = { 0 };
    //while (msg.message != WM_QUIT )
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ShowRacketAndBall();//������ ���, ������� � �����
        
        //ShowScore();//������ ���� � �����
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)

        //ProcessInput();//����� ����������
        //LimitRacket();//���������, ����� ������� �� ������� �� �����
        //ProcessBall();//���������� �����
        //ProcessRoom();//������������ ������� �� ���� � �������, ��������� ������ � ��������
    }

}
