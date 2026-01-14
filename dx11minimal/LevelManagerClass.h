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

#include "Transform.cpp"
#include "PhysicBody.cpp"
#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "SpriteCluster.cpp"
#include "Constellation.cpp"
#include "Explosion.cpp"
#include "SplinePath.cpp"
#include "Patern_Enemy_Logik.cpp"


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
	PlayerController* playerController;
	CollisionManagerClass* collisionManager;
	BaseStar* Star;
	SmallConstellation* smallConstellation;

};

#endif