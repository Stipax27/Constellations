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

const float starSize = 8;
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

HBRUSH fireBrush;
HPEN firePen;

HBRUSH earthBrush;
HPEN earthPen;

HBRUSH airBrush;
HPEN airPen;

HBRUSH waterBrush;
HPEN waterPen;


#include "utils.h"
#include "MainWindow.h"
#include "Point3d.h"
#include "mouse.h"
#include "Constellation.h"
void (*modelTransform)(point3d& p, Constellation& Constellation);
void (*modelProject)(point3d& p);
void (*uiFunc)(point3d& point, Constellation& Constellation, Entity* entity, int i);
#include "Line.h"
#include "MainWorld.h"
#include "DodgeEnemy.h"
#include "Navigation.h"
#include "StatusGame.h"
#include "font.h"
#include "Weapon.h"
#include "Elements.h"
#include "MainGame.h"
#include "Helper.h"
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
    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����

    mainBrush = CreateSolidBrush(RGB(0, 191, 255));
    mainPen = CreatePen(PS_SOLID,2,RGB(255, 255, 255));

    heroBrush = CreateSolidBrush(RGB(225, 225, 255));
    heroPen = CreatePen(PS_SOLID, 2, RGB(225, 225, 255));

    fireBrush = CreateSolidBrush(RGB(255, 100, 0));
    firePen   = CreatePen(PS_SOLID, 2, RGB(255, 100, 0));

    earthBrush = CreateSolidBrush(RGB(139, 69, 19));
    earthPen   = CreatePen(PS_SOLID, 2, RGB(139, 69, 19));

    airBrush  = CreateSolidBrush(RGB(173, 216, 230));
    airPen    = CreatePen(PS_SOLID, 2, RGB(173, 216, 230));

    waterBrush = CreateSolidBrush(RGB(0, 105, 148));
    waterPen  = CreatePen(PS_SOLID, 2, RGB(0, 105, 148));


    InitGame();//����� �������������� ���������� ����
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
        drawer::drawWorld();//������ ���, ������� � �����
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����   


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
        
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)
    }

}
