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

#include "Engine/Lib/interp.h"

#include "windowclass.h"
#include "Engine/Mouse/mouseclass.h"

#include "PlayerController.h"
#include "questmanager.h"

#include "Engine/ECS_Base/world.h"
#include "Engine/ECS_Base/system.h"
#include "Engine/ECS_Base/entity.h"
#include "Engine/ECS_Base/component.h"

#include "CollisionManagerClass.h"

#include "SplinePath.cpp"


#include "spriteSystem.h"
#include "Engine/Render/Mesh/meshSystem.h"
#include "uiSystem.h"
#include "Engine/UI/Text/UITextSystem.h"
#include "Engine/Render/Nebula/nebulaSystem.h"

#include "physicSystem.h"
#include "Engine/Physic//Gravitation/GravitySystem.h"
#include "collisionSystem.h"
#include "combatSystem.h"

#include "entityManagerSystem.h"
#include "timeSystem.h"

#include "Engine/Debug/collisionDrawSystem.h"
#include "Engine/Debug/GravityDrawSystem.h"

#include "Engine/Mesh/Animation.h"
#include "Engine/Mesh/Animator.h"
#include "Engine/Mesh/Skeleton.h"
#include "Engine/Mesh/SkinnedMesh.h"

#include "Systems/AISystem.h"

/////////////
// GLOBALS //
/////////////
//const bool VSYNC_ENABLED = true;
#define SHOW_COLLIDERS false
#define SHOW_GRAVITY true


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
	QuestManager* questManager;

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
	void CreateNebula(Entity*, int);
	void CreateStarQuestLoc(Entity*, int);
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
private: // AI amogus
	Entity* testEnemy;
	AISystem* aiSystem;

};

#endif
