#ifdef _WIN32
#include "XAudio2Platform.h"
#include <cassert>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

namespace Audio
{
    XAudio2Platform::XAudio2Platform()
        : m_xAudio2(nullptr)
        , m_masterVoice(nullptr)
        , m_x3dAudio()
        , m_listener()
        , m_masterDetails()
        , m_initialized(false)
    {
        memset(&m_listener, 0, sizeof(m_listener));
        memset(&m_masterDetails, 0, sizeof(m_masterDetails));
    }

    XAudio2Platform::~XAudio2Platform()
    {
        Shutdown();
    }

    AudioResult XAudio2Platform::Initialize()
    {
        if (m_initialized) return AudioResult::Success;

        HRESULT hr = XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr)) return AudioResult::Error_InitFailed;

        hr = m_xAudio2->CreateMasteringVoice(&m_masterVoice);
        if (FAILED(hr))
        {
            m_xAudio2->Release();
            m_xAudio2 = nullptr;
            return AudioResult::Error_InitFailed;
        }

        m_masterVoice->GetVoiceDetails(&m_masterDetails);
        DWORD channelMask;
        m_masterVoice->GetChannelMask(&channelMask);

        if (X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, m_x3dAudio) != S_OK)
        {
            Shutdown();
            return AudioResult::Error_InitFailed;
        }

        m_listener = X3DAUDIO_LISTENER();
        m_listener.OrientFront = { 0.0f, 0.0f, 1.0f };
        m_listener.OrientTop = { 0.0f, 1.0f, 0.0f };
        m_listener.Position = { 0.0f, 0.0f, 0.0f };
        m_listener.Velocity = { 0.0f, 0.0f, 0.0f };

        m_initialized = true;
        return AudioResult::Success;
    }

    void XAudio2Platform::Shutdown()
    {
        Cleanup();
    }

    void XAudio2Platform::Update()
    {
        // Обновление состояния
    }

    bool XAudio2Platform::CreateVoice(const AudioProperties& properties, void** voice)
    {
        if (!m_initialized || !voice) return false;

        WAVEFORMATEX format = {};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = properties.channels;
        format.nSamplesPerSec = properties.sampleRate;
        format.wBitsPerSample = properties.bitsPerSample;
        format.nBlockAlign = (properties.channels * properties.bitsPerSample) / 8;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

        IXAudio2SourceVoice* sourceVoice = nullptr;
        HRESULT hr = m_xAudio2->CreateSourceVoice(&sourceVoice, &format);
        if (FAILED(hr)) return false;

        VoiceData voiceData;
        voiceData.voice = sourceVoice;
        voiceData.srcChannels = properties.channels;

        voiceData.emitter = X3DAUDIO_EMITTER();
        voiceData.emitter.ChannelCount = properties.channels;
        voiceData.emitter.CurveDistanceScaler = 1.0f;
        voiceData.emitter.DopplerScaler = 1.0f;

        if (properties.channels == 1)
        {
            voiceData.emitter.pCone = nullptr;
        }

        *voice = sourceVoice;
        m_voices[sourceVoice] = voiceData;

        return true;
    }

    void XAudio2Platform::DestroyVoice(void* voice)
    {
        auto it = m_voices.find(voice);
        if (it != m_voices.end())
        {
            if (it->second.voice)
            {
                it->second.voice->DestroyVoice();
            }
            m_voices.erase(it);
        }
    }

    void XAudio2Platform::SubmitBuffer(void* voice, const uint8_t* data, uint32_t size)
    {
        auto voiceData = GetVoiceData(voice);
        if (!voiceData || !voiceData->voice) return;

        XAUDIO2_BUFFER buffer = {};
        buffer.AudioBytes = size;
        buffer.pAudioData = data;
        buffer.Flags = XAUDIO2_END_OF_STREAM;

        voiceData->voice->SubmitSourceBuffer(&buffer);
    }

    void XAudio2Platform::StartVoice(void* voice)
    {
        auto voiceData = GetVoiceData(voice);
        if (voiceData && voiceData->voice)
        {
            voiceData->voice->Start();
        }
    }

    void XAudio2Platform::StopVoice(void* voice)
    {
        auto voiceData = GetVoiceData(voice);
        if (voiceData && voiceData->voice)
        {
            voiceData->voice->Stop();
        }
    }

    void XAudio2Platform::SetVolume(void* voice, float volume)
    {
        auto voiceData = GetVoiceData(voice);
        if (voiceData && voiceData->voice)
        {
            voiceData->voice->SetVolume(volume);
        }
    }

    void XAudio2Platform::SetFrequencyRatio(void* voice, float ratio)
    {
        auto voiceData = GetVoiceData(voice);
        if (voiceData && voiceData->voice)
        {
            voiceData->voice->SetFrequencyRatio(ratio);
        }
    }

    void XAudio2Platform::SetOutputMatrix(void* voice, const float* matrix, uint32_t srcChannels, uint32_t dstChannels)
    {
        auto voiceData = GetVoiceData(voice);
        if (voiceData && voiceData->voice)
        {
            voiceData->voice->SetOutputMatrix(nullptr, srcChannels, dstChannels, matrix);
        }
    }

    void XAudio2Platform::SetListenerPosition(const Vector3& position, const Vector3& front, const Vector3& top)
    {
        m_listener.Position = { position.x, position.y, position.z };
        m_listener.OrientFront = { front.x, front.y, front.z };
        m_listener.OrientTop = { top.x, top.y, top.z };
    }

    void XAudio2Platform::CalculateSpatialParameters(void* voice, const Vector3& position, float* matrix, uint32_t dstChannels, float* dopplerFactor)
    {
        auto voiceData = GetVoiceData(voice);
        if (!voiceData || !voiceData->voice) return;

        voiceData->emitter.Position = { position.x, position.y, position.z };

        X3DAUDIO_DSP_SETTINGS dspSettings = {};
        voiceData->matrix.resize(dstChannels * voiceData->srcChannels);
        dspSettings.SrcChannelCount = voiceData->srcChannels;
        dspSettings.DstChannelCount = dstChannels;
        dspSettings.pMatrixCoefficients = voiceData->matrix.data();

        X3DAudioCalculate(m_x3dAudio, &m_listener, &voiceData->emitter,
            X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER,
            &dspSettings);

        if (matrix)
        {
            memcpy(matrix, voiceData->matrix.data(), sizeof(float) * voiceData->srcChannels * dstChannels);
        }

        if (dopplerFactor)
        {
            *dopplerFactor = dspSettings.DopplerFactor;
        }
    }

    void XAudio2Platform::Cleanup()
    {
        for (auto& voice : m_voices)
        {
            if (voice.second.voice)
            {
                voice.second.voice->DestroyVoice();
            }
        }
        m_voices.clear();

        if (m_masterVoice)
        {
            m_masterVoice->DestroyVoice();
            m_masterVoice = nullptr;
        }

        if (m_xAudio2)
        {
            m_xAudio2->Release();
            m_xAudio2 = nullptr;
        }

        m_initialized = false;
    }

    XAudio2Platform::VoiceData* XAudio2Platform::GetVoiceData(void* voice)
    {
        auto it = m_voices.find(voice);
        return it != m_voices.end() ? &it->second : nullptr;
    }
}
#endif