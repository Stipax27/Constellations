////////////////////////////////////////////////////////////////////////////////
// Filename: LevelManagerClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LEVELMANAGERCLASS_H_
#define _LEVELMANAGERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <thread>
#include <vector>

#include "windowclass.h"
#include "mouseclass.h"

#include "PlayerController.h"

#include "world.h"
#include "system.h"
#include "entity.h"
#include "component.h"
#include "Enemy_Patern.h"

#include "CollisionManagerClass.h"

#include "SplinePath.cpp"


#include "spriteSystem.h"
#include "meshSystem.h"
#include "uiSystem.h"
#include "Systems/UITextSystem.h"
#include "nebulaSystem.h"

#include "physicSystem.h"
#include "collisionSystem.h"
#include "combatSystem.h"

#include "entityManagerSystem.h"
#include "timeSystem.h"

#include "collisionDrawSystem.h"
#include "Engine/Mesh/Animation.h"
#include "Engine/Mesh/Animator.h"
#include "Engine/Mesh/Skeleton.h"
#include "Engine/Mesh/SkinnedMesh.h"

/////////////
// GLOBALS //
/////////////
//const bool VSYNC_ENABLED = true;
const bool SHOW_COLLIDERS = false;


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

	void ProcessSound(const char* name);

private:
	World* m_World;
	PlayerController* playerController;
	CollisionManagerClass* collisionManager;
	Entity* m_TestAnimEntity = nullptr;
	int m_TestAnimCycleIndex = 0;
	bool m_WasToggleAnimationPressed = false;

	void LoadModels();
	Entity* CreatePlayer(Entity* = nullptr);

	void CreateUI();
	void InitSystems();

	void CreateSpaceBackground(Entity*, int);
	void CreateAries(Entity*);
	void CreateZenithLocation(Entity*, int);
	void UpdateTestAnimationToggle();

	ID3D11Buffer* m_BoneBuffer = nullptr;
	SkinnedMesh m_FoxMesh;
	Skeleton    m_FoxSkeleton;
	std::vector<AnimationClip> m_FoxAnimations;
	SkinnedMesh m_CesiumMesh;
	Skeleton    m_CesiumSkeleton;
	std::vector<AnimationClip> m_CesiumAnimations;
	SkinnedMesh m_TestAnimMesh;
	Skeleton    m_TestAnimSkeleton;
	std::vector<AnimationClip> m_TestAnimAnimations;
};

#endif
