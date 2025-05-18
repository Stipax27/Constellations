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
#include <sstream>
#include <algorithm>
#include <deque>
#include "timer.h"

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const float starSize = 10;
int startTime;
float circleRadius;
int currentDayIndex = -1;
int currentMonthIndex = -1;
int currentColorIndex = -1;
const int numColors = 7;
float camDist = 0;

DWORD currentTime;

HBRUSH mainBrush;
HPEN mainPen;

HBRUSH heroBrush;
HPEN heroPen;


#include "utils.h"
#include "MainWindow.h"
#include "Point3d.h"
#include "mouse.h"
#include "Constellation.h"
std::vector<Entity> entities;

#include "MainWorld.h"
#include "DodgeEnemy.h"
#include "Navigation.h"
#include "StatusGame.h"
#include "font.h"
#include "Weapon.h"
#include "Elements.h"
#include "MainGame.h"
#include "DiologTEXT.h"
#include "DialogStruct.h"
#include "Parametrics.h"
#include "Player.h"
#include "drawer.h"



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    InitWindow();//здесь инициализируем все что нужно для рисования в окне

    mainBrush = CreateSolidBrush(RGB(0, 191, 255));
    mainPen = CreatePen(PS_SOLID,2,RGB(255, 255, 255));
    heroBrush = CreateSolidBrush(RGB(225, 225, 255));
    heroPen = CreatePen(PS_SOLID, 2, RGB(225, 225, 255));


    InitGame();//здесь инициализируем переменные игры
    //initContentData();
    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    MSG msg = { 0 };
    //while (msg.message != WM_QUIT )
    timer::StartCounter();
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        currentTime = timer::GetCounter();

        mouse.Input();
        drawer::drawWorld();//рисуем фон, ракетку и шарик
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно   


/*        int sz = starSet[currentEnemyID]->starsCords.size();
        for (int i = 0; i < sz; i++)
        {
            starSet[currentEnemyID]->starsCords[i].x *= .999;
            starSet[currentEnemyID]->starsCords[i].y *= .999;
            starSet[currentEnemyID]->starsCords[i].z *= .999;
        }
        */

        starfield_angles.x += .05;
        starfield_angles.y += .06;
        starfield_angles.z += .07;

        if (!isRewind) SaveCurrentState();
        
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
    }

}
