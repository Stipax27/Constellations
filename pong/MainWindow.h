
struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

float getX(float x)
{
    return x * window.width;
}

float getY(float y)
{
    return y * window.height;
}

void InitWindow()
{
    SetProcessDPIAware();

    hInst = (HINSTANCE)GetModuleHandle(0);
    auto width = GetSystemMetrics(SM_CXSCREEN);
    auto height = GetSystemMetrics(SM_CYSCREEN);
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, WndProc, 0,0, hInst, NULL, LoadCursor(NULL, IDC_ARROW), brush, NULL, "fx", NULL };
    RegisterClassEx(&wcex);
    window.hWnd = CreateWindow("fx", "fx", WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, width, height, NULL, NULL, hInst, NULL);
    UpdateWindow(window.hWnd);
    ShowWindow(window.hWnd, SW_SHOW);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEWHEEL:
    {
        float zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
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

