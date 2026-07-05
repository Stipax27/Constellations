#pragma once

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
#include "Engine/Physic/Movement/RotatingSystem.h"

#include "Engine/Physic/Gravitation/GravitySystem.h"
#include "Engine/Physic/Collision/collisionSystem.h"

#include "Engine/Compute/Combat/combatSystem.h"
#include "Engine/Compute/Combat/RayDamage/RayDamageSystem.h"

#include "Engine/Compute/DelayedDestroy/DelayedDestroySystem.h"
#include "Engine/Compute/Time/timeSystem.h"

#include "Engine/DebugTools/collisionDrawSystem.h"
#include "Engine/DebugTools/GravityDrawSystem.h"
#include "Engine/DebugTools/PivotDrawSystem.h"
#include "Engine/DebugTools/WayDrawSystem.h"

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

#include "Engine/Sound/SoundSystem.h"

#include "Systems/Transform2DDebugUI.h"

#include "Gameplay/Player/CameraTarget.h"