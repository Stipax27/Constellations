#ifndef _SOUND_SYSTEM_H_
#define _SOUND_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"
#include "SoundPlayer.h"


class SoundSystem : public System
{
public:
	SoundSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif