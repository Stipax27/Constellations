#pragma once
#include "AudioTypes.h"
#include "IAudioPlatform.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <atomic>

namespace Audio
{
    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

        AudioResult Initialize();
        void Shutdown();
        void Update(float deltaTime = 0.0f);

        bool LoadSound(const std::string& name, const std::string& filename);
        void UnloadSound(const std::string& name);
        void UnloadAllSounds();

        void PlaySound(const std::string& name, bool loop = false);
        void StopSound(const std::string& name);
        void StopAllSounds();
        void PauseSound(const std::string& name);
        void ResumeSound(const std::string& name);

        void SetVolume(const std::string& name, float volume);
        void SetPitch(const std::string& name, float pitch);
        void SetPan(const std::string& name, float pan);

        void SetSoundPosition(const std::string& name, const Vector3& position);
        void SetListenerPosition(const Vector3& position,
            const Vector3& front = Vector3(0.0f, 0.0f, 1.0f),
            const Vector3& top = Vector3(0.0f, 1.0f, 0.0f));

        void ApplyFadeIn(const std::string& name, float duration);
        void ApplyFadeOut(const std::string& name, float duration);
        void ApplyCrossFade(const std::string& from, const std::string& to, float duration);

        void PlayLayer(const std::string& name, int layerId, bool loop = false);
        void StopLayer(int layerId);
        void SetLayerVolume(int layerId, float volume);

        bool IsPlaying(const std::string& name) const;
        float GetSoundDuration(const std::string& name) const;

    private:
        struct SoundInstance
        {
            std::vector<uint8_t> data;
            AudioProperties properties;
            void* voice;
            bool isPlaying;
            float currentVolume;
            float fadeTargetVolume;
            float fadeDuration;
            float fadeTimer;
            bool isFading;
            Vector3 position;

            SoundInstance() : voice(nullptr), isPlaying(false), currentVolume(1.0f),
                fadeTargetVolume(1.0f), fadeDuration(0.0f), fadeTimer(0.0f),
                isFading(false), position() {
            }
        };

        struct FadeEffect
        {
            std::string soundName;
            float startVolume;
            float targetVolume;
            float duration;
            float timer;
            bool active;

            FadeEffect() : startVolume(0.0f), targetVolume(0.0f), duration(0.0f), timer(0.0f), active(false) {}
        };

        std::unique_ptr<IAudioPlatform> m_platform;
        std::unordered_map<std::string, SoundInstance> m_sounds;
        std::unordered_map<int, SoundInstance> m_layers;
        std::vector<FadeEffect> m_fadeEffects;
        std::atomic<bool> m_initialized;

        void UpdateFades(float deltaTime);
        bool LoadWAVFile(const std::string& filename, std::vector<uint8_t>& data, AudioProperties& properties);
        void ApplyPanning(SoundInstance& sound, float pan);
        SoundInstance* GetSoundInstance(const std::string& name);
        const SoundInstance* GetSoundInstance(const std::string& name) const;
        SoundInstance* GetLayerInstance(int layerId);
    };
}