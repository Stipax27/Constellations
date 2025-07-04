
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
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    WNDCLASSEX wcex = {sizeof(WNDCLASSEX),CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,WndProc,0, 0,hInst,NULL,LoadCursor(NULL, IDC_ARROW),brush,NULL,"fx",NULL};

    RegisterClassEx(&wcex);

    HWND hWnd = CreateWindow("fx", "fx",WS_POPUP | WS_VISIBLE | WS_MAXIMIZE,0, 0,width, height,NULL, NULL,hInst, NULL);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    RECT r;
    GetClientRect(hWnd, &r);

    window.hWnd = hWnd;
    window.device_context = GetDC(hWnd);
    window.width = r.right - r.left;
    window.height = r.bottom - r.top;

    // ? GDI буфер отключён
    window.context = nullptr;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
