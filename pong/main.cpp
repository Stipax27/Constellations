//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include "vector"
#include <stdexcept>
#include "math.h"

// секция данных игры  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//хэндл к спрайту шарика 
} sprite;
float starSize = 10;

sprite racket;//ракетка игрока
sprite enemy;//ракетка противника
sprite ball;//шарик

struct {
    int score, balls;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

HBITMAP hBack;// хэндл для фонового изображения
void rotateworld();
//cекция кода

void InitGame()
{
    //в этой секции загружаем спрайты с помощью функций gdi
    //пути относительные - файлы должны лежать рядом с .exe 
    //результат работы LoadImageA сохраняет в хэндлах битмапов, рисование спрайтов будет произовдиться с помощью этих хэндлов
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

    racket.width = 300;
    racket.height = 50;
    racket.speed = 30;//скорость перемещения ракетки
    racket.x = window.width / 2.;//ракетка посередине окна
    racket.y = window.height - racket.height;//чуть выше низа экрана - на высоту ракетки

    enemy.x = racket.x;//х координату оппонета ставим в ту же точку что и игрока

    ball.dy = (rand() % 65 + 35) / 100.;//формируем вектор полета шарика
    ball.dx = -(1 - ball.dy);//формируем вектор полета шарика
    ball.speed = 11;
    ball.rad = 20;
    ball.x = racket.x;//x координата шарика - на середие ракетки
    ball.y = racket.y - ball.rad;//шарик лежит сверху ракетки

    game.score = 0;
    game.balls = 9;

   
}

void ProcessSound(const char* name)//проигрывание аудиофайла в формате .wav, файл должен лежать в той же папке где и программа
{
    PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//переменная name содежрит имя файла. флаг ASYNC позволяет проигрывать звук паралельно с исполнением программы
}

void ShowScore()
{
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//буфер для текста
    _itoa_s(game.score, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
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

    hMemDC = CreateCompatibleDC(hDC); // Создаем контекст памяти, совместимый с контекстом отображения
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// Выбираем изображение bitmap в контекст памяти

    if (hOldbm) // Если не было ошибок, продолжаем работу
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // Определяем размеры изображения

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//все пиксели черного цвета будут интепретированы как прозрачные
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // Рисуем изображение bitmap
        }

        SelectObject(hMemDC, hOldbm);// Восстанавливаем контекст памяти
    }

    DeleteDC(hMemDC); // Удаляем контекст памяти
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
}// Добавили вращение в 3d пространстве.

void project(point3d& p)
{
    float d = 1000; // Отдаление камеры.
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
    rotateY(p, angleY); // Сферообразное пространство.
}

void genRandCube(point3d& p)
{
    float amp = 1.25;
    p.x = (rand() % window.width - window.width / 2) * amp;
    p.y = (rand() % window.width - window.width / 2) * amp;
    p.z = (rand() % window.width - window.width / 2) * amp; // Оброзование Кубического пространства.
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
bool rmb = false; //Правая кнопка мыши

void showСonstellation(std::vector <float> &starArray)
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

void arrangeСonstellation(std::vector <float>& starArray, float angleX, float angleY, float angleZ )
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
    ScreenToClient(window.hWnd, &mouse); // Управление мышью.


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

    arrangeСonstellation(Aries, 17, -50, 1 );
    arrangeСonstellation(UrsaMajor, 0, 0, 0);

    showСonstellation(Aries);
    showСonstellation(UrsaMajor);

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
        RGB(255, 0, 0),    // Красный
        RGB(255, 165, 0),  // Оранжевый
        RGB(255, 255, 0),  // Желтый
        RGB(0, 255, 0),    // Зеленый
        RGB(0, 255, 255),  // Голубой
        RGB(0, 0, 255),    // Синий
        RGB(128, 0, 128)   // Фиолетовый
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

        DeleteObject(brush);// Отрисовка цветового круга 
    }

    return;


    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//задний фон
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ракетка игрока

    if (ball.dy < 0 && (enemy.x - racket.width / 4 > ball.x || ball.x > enemy.x + racket.width / 4))
    {
        //имитируем разумность оппонента. на самом деле, компьютер никогда не проигрывает, и мы не считаем попадает ли его ракетка по шарику
        //вместо этого, мы всегда делаем отскок от потолка, а раектку противника двигаем - подставляем под шарик
        //движение будет только если шарик летит вверх, и только если шарик по оси X выходит за пределы половины длины ракетки
        //в этом случае, мы смешиваем координаты ракетки и шарика в пропорции 9 к 1
        enemy.x = ball.x * .1 + enemy.x * .9;
    }

    ShowBitmap(window.context, enemy.x - racket.width / 2, 0, racket.width, racket.height, enemy.hBitmap);//ракетка оппонента
    ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// шарик
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
    racket.x = min(racket.x, window.width - racket.width / 2.);//аналогично для правого угла
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
    if (ball.y > window.height - ball.rad - racket.height)//шарик пересек линию отскока - горизонталь ракетки
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//шарик отбит, и мы не в режиме обработки хвоста
        {
            game.score++;//за каждое отбитие даем одно очко
            ball.speed += 5. / game.score;//но увеличиваем сложность - прибавляем скорости шарику
            ball.dy *= -1;//отскок
            racket.width -= 10. / game.score;//дополнительно уменьшаем ширину ракетки - для сложности
            ProcessSound("bounce.wav");//играем звук отскока
        }
        else
        {//шарик не отбит

            tail = true;//дадим шарику упасть ниже ракетки

            if (ball.y - ball.rad > window.height)//если шарик ушел за пределы окна
            {
                game.balls--;//уменьшаем количество "жизней"

                ProcessSound("fail.wav");//играем звук

                if (game.balls < 0) { //проверка условия окончания "жизней"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//выводим сообщение о проигрыше
                    InitGame();//переинициализируем игру
                }

                ball.dy = (rand() % 65 + 35) / 100.;//задаем новый случайный вектор для шарика
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//инициализируем координаты шарика - ставим его на ракетку
                ball.y = racket.y - ball.rad;
                game.action = false;//приостанавливаем игру, пока игрок не нажмет пробел
                tail = false;
            }
        }
    }
}

void ProcessRoom()
{
    //обрабатываем стены, потолок и пол. принцип - угол падения равен углу отражения, а значит, для отскока мы можем просто инвертировать часть вектора движения шарика
    CheckWalls();
    CheckRoof();
    CheckFloor();
}

void ProcessBall()
{
    if (game.action)
    {
        //если игра в активном режиме - перемещаем шарик
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //иначе - шарик "приклеен" к ракетке
        ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowRacketAndBall();//рисуем фон, ракетку и шарик
        //drawColorCircle(window.context);
        //ShowScore();//рисуем очик и жизни
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

        //ProcessInput();//опрос клавиатуры
        //LimitRacket();//проверяем, чтобы ракетка не убежала за экран
        //ProcessBall();//перемещаем шарик
        //ProcessRoom();//обрабатываем отскоки от стен и каретки, попадание шарика в картетку
    }

}
