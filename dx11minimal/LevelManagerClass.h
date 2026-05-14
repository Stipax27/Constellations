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

#include "Engine/Window/windowclass.h"
#include "Engine/Mouse/mouseclass.h"

#include "Engine/Player/PlayerController.h"

#include "Engine/ECS_Base/world.h"
#include "Engine/ECS_Base/system.h"
#include "Engine/ECS_Base/entity.h"
#include "Engine/ECS_Base/component.h"

#include "Engine/Physic/Collision/CollisionManagerClass.h"


#include "Engine/Render/spriteSystem.h"
#include "Engine/Render/Mesh/meshSystem.h"
#include "Engine/Render/Nebula/nebulaSystem.h"
#include "Engine/Render/StarClay/StarClaySystem.h"

#include "Engine/UI/uiSystem.h"
#include "Engine/UI/Text/UITextSystem.h"

#include "Engine/Physic/Movement/physicSystem.h"
#include "Engine/Physic/Gravitation/GravitySystem.h"
#include "Engine/Physic/Collision/collisionSystem.h"

#include "Engine/Compute/Combat/combatSystem.h"
#include "Engine/Compute/Combat/RayDamageSystem.h"

#include "Engine/Compute/DelayedDestroy/DelayedDestroySystem.h"
#include "Engine/Compute/Time/timeSystem.h"

#include "Engine/DebugTools/collisionDrawSystem.h"
#include "Engine/DebugTools/GravityDrawSystem.h"

#include "Engine/Mesh/Animation.h"
#include "Engine/Mesh/Animator.h"
#include "Engine/Mesh/Skeleton.h"
#include "Engine/Render/SkinnedMesh/SkinnedMesh.h"

#include "Engine/Compute/AI/AISystem.h"

#include "Engine/Render/BoneAnimation/SkeletalAnimationComponent.h"
#include "Engine/Render/BoneAnimation/SkeletalAnimationSystem.h"
#include "Engine/Render/SkinnedMesh/SkinnedMeshSystem.h"

#include "Engine/Compute/Quests/questmanager.h"
#include "Engine/Compute/Quests/QuestSystem.h"

/////////////
// GLOBALS //
/////////////
//const bool VSYNC_ENABLED = true;
#define SHOW_COLLIDERS true
#define SHOW_GRAVITY false


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
	void CreateArenaBarrier(Entity* parent, const point3d& center, float radius, int starCount);
	void ShowGameOverMessage(const wchar_t* message, const point3d& color);

	bool m_IsExecutionActive = false;
	Entity* m_ExecutionUI = nullptr;
	float m_ExecutionTimer = 0.0f;
	float m_ExecutionTimeLimit = 5.0f;

	void TriggerExecution();
	void ExecuteBoss();
	void ShowExecutionUI();
	void HideExecutionUI();

	ID3D11Buffer* m_BoneBuffer = nullptr;
	SkinnedMesh m_FoxMesh;
	Skeleton    m_FoxSkeleton;
	std::vector<AnimationClip> m_FoxAnimations;
	SkinnedMesh m_CesiumMesh;
	Skeleton    m_CesiumSkeleton;
	std::vector<AnimationClip> m_CesiumAnimations;


	SkinnedMesh m_PunchComboNewMesh;
	Skeleton    m_PunchComboNewSkeleton;
	std::vector<AnimationClip> m_PunchComboNewAnimations;

	SkinnedMesh m_TestAnimMesh;
	Skeleton    m_TestAnimSkeleton;
	std::vector<AnimationClip> m_TestAnimAnimations;
private: // AI amogus
	Entity* testEnemy;
	Entity* worldFolder;
	Entity* m_CurrentBoss;
	Entity* m_BossHealthFill;      // Полоска здоровья
	TextLabel* m_BossNumbersText;  // Текст с цифрами
	TextLabel* m_BossNameText;     // Текст с именем
	bool m_IsInBossArena = false;
	float m_BossArenaRadius = 60.0f;
	point3d m_BossArenaCenter = point3d(0, 0, 0);

	bool m_ShowGameOverMessage = false;
	bool m_ShowVictoryMessage = false;
	float m_MessageTimer = 0.0f;
	
};

#endif
