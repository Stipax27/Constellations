//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include "vector"
#include <stdexcept>
#include "math.h"

// ������ ������ ����  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//����� � ������� ������ 
} sprite;
float starSize = 10;

sprite racket;//������� ������
sprite enemy;//������� ����������
sprite ball;//�����

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
void rotateworld();
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

float getZ(float z)
{
    return z * window.width;
}

float saturate(float x)
{
    return max(min(x, 1.), 0.);
}

typedef struct {
    float x;
    float y;
    float z;
} point3d;

point3d point;
void move(point3d& p, float x, float y, float z)
{
    p.x += x;
    p.y += y;
    p.z += z;
}

void rotateX(point3d& p, float angle)
{
    float a = angle * 3.14 / 180.;

    float x1 = p.x;
    float y1 = p.y * cos(a) - p.z * sin(a);
    float z1 = p.y * sin(a) + p.z * cos(a);

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

void rotateY(point3d &p, float angle)
{
    float a = angle * 3.14 / 180.;

    float x1 = p.x * cos(a) - p.z * sin(a);
    float y1 = p.y;
    float z1 = p.x * sin(a) + p.z * cos(a);

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

void rotateZ(point3d& p, float angle)
{
    float a = angle * 3.14 / 180.;

    float x1 = p.x * cos(a) - p.y * sin(a);
    float y1 = p.x * sin(a) + p.y * cos(a);
    float z1 = p.z;

    p.x = x1;
    p.y = y1;
    p.z = z1;
}// �������� �������� � 3d ������������.

void project(point3d& p)
{
    float d = 1000; // ��������� ������.
    float x = window.width / 2. + point.x * d / (point.z + d);
    float y = window.height / 2. + point.y * d / (point.z + d);
    
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
    p.z = window.width/3;
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
    );
}


POINT mouse;
point3d mouseAngle; 
point3d oldmouse;
point3d oldmouseAngle;
bool rmb = false; //������ ������ ����

void show�onstellation(std::vector <float> &starArray)
{
    int starsCount = starArray.size()/3;

    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
    HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
    HBRUSH brush2 = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(window.context, pen);
    SelectObject(window.context, brush);

    for (int i = 0; i < starsCount; i++)
   {
        point.x = starArray[i * 3];
        point.y = starArray[i * 3 + 1];
        point.z = starArray[i * 3 + 2];

        float a = timeGetTime() * .01;
        rotateworld();
        project(point);

        if (i == 0)
        {
            MoveToEx(window.context, point.x, point.y, NULL);
        }
        else
        {
            LineTo(window.context, point.x, point.y);
        }
    }

    for (int i = 0; i < starsCount; i++)
    {
        point.x = starArray[i * 3];
        point.y = starArray[i * 3 + 1];
        point.z = starArray[i * 3 + 2];

        float a = timeGetTime() * .01;
        rotateworld();
        project(point);

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

        SelectObject(window.context, brush);

        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (lenght < starSize)
            {
                SelectObject(window.context, brush2);
            }
        }

        float sz = starSize + rad * 15;
        drawPoint(point, sz);

    }


    DeleteObject(pen);
    DeleteObject(brush);
    DeleteObject(brush2);
}

void arrange�onstellation(std::vector <float>& starArray, float angleX, float angleY, float angleZ )
{
    int starsCount = starArray.size() / 3;
    float scale = 1000;
    for (int i = 0; i < starsCount; i++)
    {
        
        point3d p = { starArray[i * 3 + 0], starArray[i * 3 + 1], starArray[i * 3 + 2] };

        move(p, 0, 0, 1000./scale);
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

    }
}

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
    else { rmb = false; }

    RECT rect;
    GetClientRect(window.hWnd, &rect);
    auto blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(window.context, &rect, blackBrush);
    DeleteObject(blackBrush);

    showStarField();

    std::vector <float> Aries = { 0, 0, 0., .21, .05, 0., .35, .12, 0., .43, .27, 0. };
    std::vector <float> UrsaMajor = { -.25, -.25, 0., -.1, -.2, 0.,-.05, .05, 0.,.25, .05,0., .35, .25,0., .05, .25,0., -.05, .05,0. };
    //std::vector <float> 

    arrange�onstellation(Aries, 17, -50, 1 );
    arrange�onstellation(UrsaMajor, 0, 0, 0);

    show�onstellation(Aries);
    show�onstellation(UrsaMajor);

    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
    HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
    SelectObject(window.context, pen);
    SelectObject(window.context, brush);

    Ellipse(window.context,
        mouse.x - starSize / 2,
        mouse.y - starSize / 2,
        mouse.x + starSize / 2,
        mouse.y + starSize / 2
    );

    DeleteObject(pen);
    DeleteObject(brush);
}
void drawColorCircle(HDC hdc) {
    const COLORREF colors[] = {
        RGB(255, 0, 0),    // �������
        RGB(255, 165, 0),  // ���������
        RGB(255, 255, 0),  // ������
        RGB(0, 255, 0),    // �������
        RGB(0, 255, 255),  // �������
        RGB(0, 0, 255),    // �����
        RGB(128, 0, 128)   // ����������
    };

    float circleRadius = 75;
    float centerX = window.width / 2;
    float centerY = window.height / 2;

    int numColors = sizeof(colors) / sizeof(colors[0]);
    float angleStep = 2 * 3.14 / (float)numColors;

    for (int i = 0; i < numColors; ++i) {

        float angle1 = -i * angleStep;
        float angle2 = -(i + 1) * angleStep;

        POINT p1, p2;
        p1.x = centerX + circleRadius * cos(angle1);
        p1.y = centerY + circleRadius * sin(angle1);

        p2.x = centerX + circleRadius * cos(angle2);
        p2.y = centerY + circleRadius * sin(angle2);


        HBRUSH brush = CreateSolidBrush(colors[i]);
        SelectObject(hdc, brush);


        Pie(hdc,
            centerX - circleRadius,
            centerY - circleRadius,
            centerX + circleRadius,
            centerY + circleRadius,
            p1.x, p1.y,
            p2.x, p2.y);

        DeleteObject(brush);// ��������� ��������� ����� 
    }

    return;


    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//������ ���
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ������� ������

    if (ball.dy < 0 && (enemy.x - racket.width / 4 > ball.x || ball.x > enemy.x + racket.width / 4))
    {
        //��������� ���������� ���������. �� ����� ����, ��������� ������� �� �����������, � �� �� ������� �������� �� ��� ������� �� ������
        //������ �����, �� ������ ������ ������ �� �������, � ������� ���������� ������� - ����������� ��� �����
        //�������� ����� ������ ���� ����� ����� �����, � ������ ���� ����� �� ��� X ������� �� ������� �������� ����� �������
        //� ���� ������, �� ��������� ���������� ������� � ������ � ��������� 9 � 1
        enemy.x = ball.x * .1 + enemy.x * .9;
    }

    ShowBitmap(window.context, enemy.x - racket.width / 2, 0, racket.width, racket.height, enemy.hBitmap);//������� ���������
    ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// �����
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//���� ��������� ������ ���� ������� ������ ����, �������� �� ����
    racket.x = min(racket.x, window.width - racket.width / 2.);//���������� ��� ������� ����
}

void CheckWalls()
{
    if (ball.x < ball.rad || ball.x > window.width - ball.rad)
    {
        ball.dx *= -1;
        ProcessSound("bounce.wav");
    }
}

void CheckRoof()
{
    if (ball.y < ball.rad + racket.height)
    {
        ball.dy *= -1;
        ProcessSound("bounce.wav");
    }
}

bool tail = false;
void rotateworld()
{
    rotateX(point, mouseAngle.y * 0.1);
    rotateY(point, mouseAngle.x * 0.1);
    //rotateZ(point, timeGetTime() * 0.01);
}
void CheckFloor()
{
    if (ball.y > window.height - ball.rad - racket.height)//����� ������� ����� ������� - ����������� �������
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//����� �����, � �� �� � ������ ��������� ������
        {
            game.score++;//�� ������ ������� ���� ���� ����
            ball.speed += 5. / game.score;//�� ����������� ��������� - ���������� �������� ������
            ball.dy *= -1;//������
            racket.width -= 10. / game.score;//������������� ��������� ������ ������� - ��� ���������
            ProcessSound("bounce.wav");//������ ���� �������
        }
        else
        {//����� �� �����

            tail = true;//����� ������ ������ ���� �������

            if (ball.y - ball.rad > window.height)//���� ����� ���� �� ������� ����
            {
                game.balls--;//��������� ���������� "������"

                ProcessSound("fail.wav");//������ ����

                if (game.balls < 0) { //�������� ������� ��������� "������"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//������� ��������� � ���������
                    InitGame();//������������������ ����
                }

                ball.dy = (rand() % 65 + 35) / 100.;//������ ����� ��������� ������ ��� ������
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//�������������� ���������� ������ - ������ ��� �� �������
                ball.y = racket.y - ball.rad;
                game.action = false;//���������������� ����, ���� ����� �� ������ ������
                tail = false;
            }
        }
    }
}

void ProcessRoom()
{
    //������������ �����, ������� � ���. ������� - ���� ������� ����� ���� ���������, � ������, ��� ������� �� ����� ������ ������������� ����� ������� �������� ������
    CheckWalls();
    CheckRoof();
    CheckFloor();
}

void ProcessBall()
{
    if (game.action)
    {
        //���� ���� � �������� ������ - ���������� �����
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //����� - ����� "��������" � �������
        ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//�� ������ ���� ������� ����� ��������� ���������� ��� ���������
    window.width = r.right - r.left;//���������� ������� � ���������
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//������ �����
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//����������� ���� � ���������
    GetClientRect(window.hWnd, &r);

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
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowRacketAndBall();//������ ���, ������� � �����
        //drawColorCircle(window.context);
        //ShowScore();//������ ���� � �����
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)

        //ProcessInput();//����� ����������
        //LimitRacket();//���������, ����� ������� �� ������� �� �����
        //ProcessBall();//���������� �����
        //ProcessRoom();//������������ ������� �� ���� � �������, ��������� ������ � ��������
    }

}
