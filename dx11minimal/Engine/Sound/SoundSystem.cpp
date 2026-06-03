#include "SoundSystem.h"
#include "../Lib/logging.h"

using namespace std;


SoundSystem::SoundSystem()
{
}


void SoundSystem::Initialize()
{
}


void SoundSystem::Shutdown()
{
}


void SoundSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<SoundPlayer>();
	size_t size = entities.size();
	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
			continue;

		SoundPlayer* soundPlayer = entity->GetComponent<SoundPlayer>();
		if (!soundPlayer || !soundPlayer->active)
			continue;

		if (soundPlayer->playing) {
			if (soundPlayer->pVoice == nullptr) {
				IXAudio2SourceVoice* pVoice = Audio::Play(soundPlayer->soundName);
				if (pVoice) {
					soundPlayer->pVoice = pVoice;
				}
				else {
					Log("Missing file for sound with name ");
					Log(soundPlayer->soundName.c_str());
					Log("\n");
					soundPlayer->active = false;
				}
			}
		}
		else {
			if (soundPlayer->pVoice != nullptr) {

			}
		}
	}

	Audio::UpdateVoices();
}