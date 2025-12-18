////////////////////////////////////////////////////////////////////////////////
// Filename: LevelManagerClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LEVELMANAGERCLASS_H_
#define _LEVELMANAGERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include "windowclass.h"
#include "mouseclass.h"

#include "PlayerController.h"

#include "world.h"

#include "SpriteSystem.cpp"
#include "MeshSystem.cpp"
#include "UISystem.cpp"

#include "PhysicSystem.cpp"
#include "CollisionSystem.cpp"
#include "CombatSystem.cpp"
#include "EntityManagerSystem.cpp"
#include "Star.cpp"

#include "CollisionDrawSystem.cpp"

/////////////
// GLOBALS //
/////////////
//const bool VSYNC_ENABLED = true;


////////////////////////////////////////////////////////////////////////////////
// Class name: LevelManagerClass
////////////////////////////////////////////////////////////////////////////////


class LevelManagerClass
{
public:
	WindowClass* window;
	MouseClass* mouse;

public:
	LevelManagerClass();
	LevelManagerClass(const LevelManagerClass&);
	~LevelManagerClass();

	void InitWindow();
	bool Initialize();
	void Shutdown();

	void Frame();

	void ProcessSound(const char* name)//???????????? ?????????? ? ??????? .wav, ???? ?????? ?????? ? ??? ?? ????? ??? ? ?????????
	{
		//PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//?????????? name ???????? ??? ?????. ???? ASYNC ????????? ??????????? ???? ?????????? ? ??????????? ?????????
	}

private:
	World* m_World;
	PlayerController* playerController;
};

#endif