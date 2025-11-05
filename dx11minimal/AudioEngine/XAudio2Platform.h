#pragma once
#include "IAudioPlatform.h"

#ifdef _WIN32
#include <xaudio2.h>
#include <x3daudio.h>
#include <unordered_map>
#include <vector>
#include <cstring>

namespace Audio
{
    class XAudio2Platform : public IAudioPlatform
    {
    public:
        XAudio2Platform();
        virtual ~XAudio2Platform();

        AudioResult Initialize() override;
        void Shutdown() override;
        void Update() override;

        bool CreateVoice(const AudioProperties& properties, void** voice) override;
        void DestroyVoice(void* voice) override;
        void SubmitBuffer(void* voice, const uint8_t* data, uint32_t size) override;
        void StartVoice(void* voice) override;
        void StopVoice(void* voice) override;

        void SetVolume(void* voice, float volume) override;
        void SetFrequencyRatio(void* voice, float ratio) override;
        void SetOutputMatrix(void* voice, const float* matrix, uint32_t srcChannels, uint32_t dstChannels) override;

        void SetListenerPosition(const Vector3& position, const Vector3& front, const Vector3& top) override;
        void CalculateSpatialParameters(void* voice, const Vector3& position, float* matrix, uint32_t dstChannels, float* dopplerFactor) override;

    private:
        struct VoiceData
        {
            IXAudio2SourceVoice* voice;
            X3DAUDIO_EMITTER emitter;
            std::vector<float> matrix;
            uint32_t srcChannels;

            VoiceData() : voice(nullptr), srcChannels(0)
            {
                memset(&emitter, 0, sizeof(emitter));
            }
        };

        IXAudio2* m_xAudio2;
        IXAudio2MasteringVoice* m_masterVoice;
        X3DAUDIO_HANDLE m_x3dAudio;
        X3DAUDIO_LISTENER m_listener;
        std::unordered_map<void*, VoiceData> m_voices;
        XAUDIO2_VOICE_DETAILS m_masterDetails;
        bool m_initialized;

        void Cleanup();
        VoiceData* GetVoiceData(void* voice);
    };
}
#endif