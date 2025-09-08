#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wtypes.h>

class WindowClass {
public:
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
};

#endif