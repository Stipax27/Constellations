#ifndef _INPUT_H_
#define _INPUT_H_

#include "../dx11.h"
#include <vector>

namespace input
{
	//extern vector<int> _pressedKeys;

	bool IsKeyDown(const int key);
	bool IsKeyPressed(const int key);
	bool IsKeyReleased(const int key);
}

#endif
