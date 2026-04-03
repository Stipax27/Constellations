////////////////////////////////////////////////////////////////////////////////
// Filename: system.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include "Engine/BasicComponents/component.h"
#include "entity.h"
#include "entityStorage.h"

#include "dx11.h"
#include "singleton.h"
#include "componentutils.h"

/////////////
// GLOBALS //
/////////////
#define SPACE_DENSITY 1.0f

#define HIGH_RENDER_DISTANCE 10.0f
#define LOW_RENDER_DISTANCE 100.0f

const float RENDER_DISTANCE_DELTA = LOW_RENDER_DISTANCE - HIGH_RENDER_DISTANCE;


////////////////////////////////////////////////////////////////////////////////
// Class name: System
////////////////////////////////////////////////////////////////////////////////
class System
{
public:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(EntityStorage& entityStorage, float deltaTime) = 0;
};

#endif