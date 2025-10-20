#ifndef _WINDOWCLASS_H_
#define _WINDOWCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: WindowClass
////////////////////////////////////////////////////////////////////////////////
class WindowClass
{
public:
	WindowClass();
	WindowClass(const WindowClass&);
	~WindowClass();

	void Initialize();

public:
	HWND hWnd; // window handle
	HDC device_context, context; // 2 device contexts (for bufferization)
	int width, height; // saving window sizes the program created
	float aspect, iaspect;

};

#endif