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

#include "ecsHeaders.h"

#ifdef _EDITOR
#include "EditorTools/EditCameraController.h"
#else
#include "Gameplay/Player/PlayerController.h"
#endif

#include "Gameplay/MazeLinkSystem.h"


/////////////
// GLOBALS //
/////////////
//const bool VSYNC_ENABLED = true;
#define SHOW_COLLIDERS false
#define SHOW_GRAVITY false
#define SHOW_PIVOTS false
#define SHOW_LINK_RADIUS true


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

private:
	World* m_World;
	EntityStorage* entityStorage;

#ifdef _EDITOR
	EditCameraController* editCameraController;
#else
	PlayerController* playerController;
#endif

	CollisionManagerClass* collisionManager;
	QuestManager* questManager;

	Entity* m_TestAnimEntity = nullptr;
	Transform2DDebugUI m_Transform2DDebugUI;
	int m_TestAnimCycleIndex = 0;
	bool m_WasToggleAnimationPressed = false;

	void LoadModels();
	void LoadTextures();
	void LoadSounds();

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
