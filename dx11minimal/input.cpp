#include "input.h"


vector<int> input::_pressedKeys;


bool input::IsKeyDown(const int key) {
    return GetAsyncKeyState(key) & 0x8000;
}


bool input::IsKeyPressed(const int key) {
    bool isDown = IsKeyDown(key);

    if (isDown) {
        for (const int _key : _pressedKeys) {
            if (key == _key)
                return false;
        }

        if (find(_pressedKeys.begin(), _pressedKeys.end(), key) == _pressedKeys.end())
            _pressedKeys.push_back(key);

        return true;
    }
    else if (_pressedKeys.size() > 0) {
        for (int i = 0; i < _pressedKeys.size(); i++) {
            const int _key = _pressedKeys[i];
            if (key == _key) {
                _pressedKeys.erase(_pressedKeys.begin() + i);
                break;
            }
        }
    }

    return false;
}


bool input::IsKeyReleased(const int key) {
    bool isUp = !IsKeyDown(key);

    if (isUp) {
        if (_pressedKeys.size() > 0) {
            for (int i = 0; i < _pressedKeys.size(); i++) {
                const int _key = _pressedKeys[i];
                if (key == _key) {
                    _pressedKeys.erase(_pressedKeys.begin() + i);
                    return true;
                }
            }
        }
    }
    else {
        if (find(_pressedKeys.begin(), _pressedKeys.end(), key) == _pressedKeys.end())
            _pressedKeys.push_back(key);
    }

    return false;
}