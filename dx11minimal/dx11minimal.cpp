#define _CRT_SECURE_NO_WARNINGS

const float PI = 3.1415926535897;
#include "framework.h"
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
#include <d2d1.h>
#include <mmsystem.h> 
#include <random>
#pragma comment(lib, "d2d1.lib")

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

float starSize = 30;
int startTime;
float circleRadius;
int currentDayIndex = -1;
int currentMonthIndex = -1;
int currentColorIndex = -1;
const int numColors = 7;
float camDist = 100;//we have this in camera state
DWORD currentTime;
DWORD lastFrameTime = 0;
DWORD deltaTime = 0;
ID2D1SolidColorBrush* d2dBrush = nullptr;
bool isBattleActive = false;
DWORD battleStartTime;

 float heroScale = 1.f;
 //float targetHeroScale = 1.5f;
 const float MIN_HERO_SCALE = 1.0f;
 const float MAX_HERO_SCALE = 15.5f;
 const float SCALE_SPEED = 2000.f;

 float currentDamage = 1.f;


 float targetCameraDistance = 600.f; // Целевое расстояние камеры
 float currentCameraDistance = 600.f; // Текущее расстояние камеры
 float cameraReturnSpeed = .005f; // Скорость возврата камеры

 float targetHeroScale = MIN_HERO_SCALE; // Целевой масштаб героя
 float currentHeroScale = MIN_HERO_SCALE; // Текущий масштаб героя
 

 DWORD attackEndTime = 0; // Время окончания атаки
 const DWORD returnDuration = 5000; // Длительность возврата в мс



HWND hWnd;
#include "utils.h"
#include "dx11.h"
#include "Player.h"

#include "Point3d.h"
#include "RenderObject.h"

#include "AriesNebula.h"

#include "MainWindow.h"
#include "mouse.h"
#include "UI.h"
#include "font.h"

#include "DodgeEnemy.h"
#include "EnemyData.h"
#include "EnemyDataCon.h"
#include "EnemyAi.h"

#include "Navigation.h"
#include "Constellation.h"

#include "EnemyAiUPD.h"

#include "MainWorld.h"
#include "StatusGame.h"
#include "Weapon.h"
#include "Elements.h"
#include "MainGame.h"
#include "DiologTEXT.h"
#include "DialogStruct.h"
#include "Parametrics.h"
#include "drawer.h"

#include "loop.h"

#include "dx11minimal.h"


#define MAX_LOADSTRING 100
                            // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
    if (FAILED(hr)) {
        MessageBox(NULL, "Failed to create D2D factory", "Error", MB_OK);
        return -1;
    }
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX11MINIMAL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    //InitWindow();
    Dx11Init();
    InitGame();
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX11MINIMAL));

    MSG msg = { 0 };

    timer::StartCounter();
    ShowCursor(FALSE);

    // Main message loop:
    while (msg.message != WM_QUIT)
    {
        double time = timer::GetCounter();

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (time >= timer::nextFrameTime)
        {
            currentTime= timer::GetCounter();
            deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;

            timer::frameBeginTime = timer::GetCounter();
            mouse.Input();
            mainLoop(deltaTime);
            timer::frameEndTime = timer::GetCounter();
            timer::frameRenderingDuration = timer::frameEndTime - timer::frameBeginTime;
            timer::nextFrameTime = timer::frameBeginTime + FRAME_LEN;
            //Camera::Camera();//добавили общий вызов обновления камеры, чтобы везде не коллить
        }

        //Sleep((DWORD)min(FRAME_LEN, max(FRAME_LEN - timer::frameRenderingDuration, 0)));
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX11MINIMAL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX11MINIMAL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    SetProcessDPIAware();

    auto width = GetSystemMetrics(SM_CXSCREEN);
    auto height = GetSystemMetrics(SM_CYSCREEN);

    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
    RegisterClassEx(&wcex);

    window.hWnd = CreateWindow("fx", "fx", WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, width, height, NULL, NULL, hInst, NULL);
    if (!window.hWnd) return FALSE;

    ShowWindow(window.hWnd, SW_SHOW);
    UpdateWindow(window.hWnd);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);
    window.width = r.right - r.left;
    window.height = r.bottom - r.top;

    hWnd = window.hWnd; // <- чтобы DirectX тоже использовал это окно

    return TRUE;
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        break;

    case WM_MOUSEWHEEL:
    {
        float zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        Camera::HandleMouseWheel(zDelta);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
