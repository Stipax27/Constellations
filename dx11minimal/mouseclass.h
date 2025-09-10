#ifndef MOUSECLASS_H
#define MOUSECLASS_H

//////////////
// INCLUDES //
//////////////
#include "Point3d.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: MouseClass
////////////////////////////////////////////////////////////////////////////////
class MouseClass
{
public:
    point3d pos;
    point3d oldPos;
    point3d worldPos;

    void Initialize();
    void Update();
    point3d GetMouseRay(point3d);
private:
    
};

void ProcessSound(const char* name)//проигрывание аудиофайла в формате .wav, файл должен лежать в той же папке где и программа
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//переменная name содежрит имя файла. флаг ASYNC позволяет проигрывать звук паралельно с исполнением программы
}

#endif