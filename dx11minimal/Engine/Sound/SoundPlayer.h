#ifndef _SOUND_PLAYER_H_
#define _SOUND_PLAYER_H_

#include "../ECS_Base/component.h"


struct SoundPlayer : Component
{
	std::string soundName = "";
	float volume = 1.0f;

	//bool looped = false;
	bool playing = false;

	// Technical fields //
	IXAudio2SourceVoice* pVoice = nullptr;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SoundPlayer,
	active,
	soundName,
	volume,
	playing)

#endif