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

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ������ ������ ����  
struct point3d {

    float x, y, z;

    /*bool operator==(const point3d& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator==(const point3d& other) const {
        return fabs(x - other.x) < 0.001f &&
            fabs(y - other.y) < 0.001f &&
            fabs(z - other.z) < 0.001f;
    }*/
};

const float starSize = 10;
int startTime;
float circleRadius;
int currentDayIndex = -1;
int currentMonthIndex = -1;
int currentColorIndex = -1;
const int numColors = 7;
float camDist = 0;

DWORD currentTime;

#include "utils.h"
#include "MainWindow.h"
#include "mouse.h"
#include "Constellation.h"
#include "MainWorld.h"
#include "Weapon.h"
#include "Navigation.h"
#include "StatusGame.h"
#include "MainGame.h"
#include "font.h"
#include "DialogStruct.h"
#include "drawer.h"




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����
    InitGame();//����� �������������� ���������� ����
    //initContentData();
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
        currentTime = timeGetTime();

        mouseInput();
        drawer::drawWorld();//������ ���, ������� � �����
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����   
        //SaveCurrentState();
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)
    }

}
