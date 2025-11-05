#pragma once
#include "AudioTypes.h"
#include <cstdint>

namespace Audio
{
    class IAudioPlatform
    {
    public:
        virtual ~IAudioPlatform() = default;

        virtual AudioResult Initialize() = 0;
        virtual void Shutdown() = 0;
        virtual void Update() = 0;

        virtual bool CreateVoice(const AudioProperties& properties, void** voice) = 0;
        virtual void DestroyVoice(void* voice) = 0;
        virtual void SubmitBuffer(void* voice, const uint8_t* data, uint32_t size) = 0;
        virtual void StartVoice(void* voice) = 0;
        virtual void StopVoice(void* voice) = 0;

        virtual void SetVolume(void* voice, float volume) = 0;
        virtual void SetFrequencyRatio(void* voice, float ratio) = 0;
        virtual void SetOutputMatrix(void* voice, const float* matrix, uint32_t srcChannels, uint32_t dstChannels) = 0;

        virtual void SetListenerPosition(const Vector3& position, const Vector3& front, const Vector3& top) = 0;
        virtual void CalculateSpatialParameters(void* voice, const Vector3& position, float* matrix, uint32_t dstChannels, float* dopplerFactor) = 0;
    };
}