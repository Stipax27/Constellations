#define _CRT_SECURE_NO_WARNINGS

const float PI = 3.1415926535897;

#include "windows.h"
#include "vector"
#include <stdexcept>
#include "math.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// секция данных игры  
typedef struct {
    float x;
    float y;
    float z;
} point3d;

const float starSize = 10;
int startTime;
float circleRadius;
int currentDayIndex = -1;
int currentMonthIndex = -1;
int currentColorIndex = -1;
const int numColors = 7;
float camDist = 0;

#include "utils.h"
#include "MainWindow.h"
#include "mouse.h"
#include "Constellation.h"
#include "MainWorld.h"
#include "MainGame.h"

#include "DialogStruct.h"
#include "drawer.h"



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры
    initContentData();
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
        mouseInput();
        drawer::drawWorld();//рисуем фон, ракетку и шарик
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
    }

}
