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
#include "system.h"
#include "entity.h"
#include "component.h"
#include "Enemy_Patern.h"
#include "BaseStar.h"
#include "SmallConstellation.h"

#include "CollisionManagerClass.h"

#include "SplinePath.cpp"
#include "Patern_Enemy_Logik.cpp"


#include "spriteSystem.h"
#include "meshSystem.h"
#include "uiSystem.h"

#include "physicSystem.h"
#include "collisionSystem.h"
#include "combatSystem.h"
#include "entityManagerSystem.h"

#include "collisionDrawSystem.h"

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
	CollisionManagerClass* collisionManager;
	BaseStar* bStar;
	SmallConstellation* smallConstellation;

};

#endif