#include "windowclass.h"

WindowClass::WindowClass()
{
}

WindowClass::WindowClass(const WindowClass& other)
{
}


WindowClass::~WindowClass()
{
}


void WindowClass::Initialize()
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	device_context = GetDC(hWnd);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	aspect = float(height) / float(width);
	iaspect = float(width) / float(height);
}


bool WindowClass::IsActive()
{
	return GetActiveWindow() == hWnd;
}


bool WindowClass::IsFocused()
{
	return GetFocus() == hWnd || GetActiveWindow() == hWnd;
}