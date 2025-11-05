#include "AudioEngine.h"
#include "XAudio2Platform.h"
#include <fstream>
#include <algorithm>
#include <cstring>

namespace Audio
{
    AudioEngine::AudioEngine()
        : m_initialized(false)
    {
#ifdef _WIN32
        m_platform = std::make_unique<XAudio2Platform>();
#endif
    }

    AudioEngine::~AudioEngine()
    {
        Shutdown();
    }

    AudioResult AudioEngine::Initialize()
    {
        if (m_initialized) return AudioResult::Success;
        if (!m_platform) return AudioResult::Error_PlatformNotSupported;

        auto result = m_platform->Initialize();
        if (result == AudioResult::Success)
        {
            m_initialized = true;
        }

        return result;
    }

    void AudioEngine::Shutdown()
    {
        if (!m_initialized) return;

        StopAllSounds();
        UnloadAllSounds();
        if (m_platform)
        {
            m_platform->Shutdown();
        }
        m_initialized = false;
    }

    void AudioEngine::Update(float deltaTime)
    {
        if (!m_initialized) return;

        if (m_platform)
        {
            m_platform->Update();
        }
        UpdateFades(deltaTime);
    }

    bool AudioEngine::LoadSound(const std::string& name, const std::string& filename)
    {
        if (!m_initialized || !m_platform) return false;

        SoundInstance sound;
        if (!LoadWAVFile(filename, sound.data, sound.properties))
        {
            return false;
        }

        if (!m_platform->CreateVoice(sound.properties, &sound.voice))
        {
            return false;
        }

        m_platform->SubmitBuffer(sound.voice, sound.data.data(), static_cast<uint32_t>(sound.data.size()));

        sound.isPlaying = false;
        sound.currentVolume = 1.0f;
        sound.position = Vector3();

        m_sounds[name] = std::move(sound);
        return true;
    }

    void AudioEngine::UnloadSound(const std::string& name)
    {
        auto it = m_sounds.find(name);
        if (it != m_sounds.end())
        {
            if (m_platform)
            {
                m_platform->DestroyVoice(it->second.voice);
            }
            m_sounds.erase(it);
        }
    }

    void AudioEngine::UnloadAllSounds()
    {
        for (auto& pair : m_sounds)
        {
            if (m_platform)
            {
                m_platform->DestroyVoice(pair.second.voice);
            }
        }
        m_sounds.clear();

        for (auto& pair : m_layers)
        {
            if (m_platform)
            {
                m_platform->DestroyVoice(pair.second.voice);
            }
        }
        m_layers.clear();
    }

    void AudioEngine::PlaySound(const std::string& name, bool loop)
    {
        auto sound = GetSoundInstance(name);
        if (!sound || !m_platform) return;

        m_platform->StartVoice(sound->voice);
        sound->isPlaying = true;
    }

    void AudioEngine::StopSound(const std::string& name)
    {
        auto sound = GetSoundInstance(name);
        if (sound && m_platform)
        {
            m_platform->StopVoice(sound->voice);
            sound->isPlaying = false;
        }
    }

    void AudioEngine::StopAllSounds()
    {
        for (auto& pair : m_sounds)
        {
            if (m_platform)
            {
                m_platform->StopVoice(pair.second.voice);
            }
            pair.second.isPlaying = false;
        }

        for (auto& pair : m_layers)
        {
            if (m_platform)
            {
                m_platform->StopVoice(pair.second.voice);
            }
            pair.second.isPlaying = false;
        }
    }

    void AudioEngine::PauseSound(const std::string& name)
    {
        StopSound(name); // Временная реализация
    }

    void AudioEngine::ResumeSound(const std::string& name)
    {
        auto sound = GetSoundInstance(name);
        if (sound && m_platform)
        {
            m_platform->StartVoice(sound->voice);
            sound->isPlaying = true;
        }
    }

    void AudioEngine::SetVolume(const std::string& name, float volume)
    {
        auto sound = GetSoundInstance(name);
        if (sound && m_platform)
        {
            sound->currentVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
            m_platform->SetVolume(sound->voice, sound->currentVolume);
        }
    }

    void AudioEngine::SetPitch(const std::string& name, float pitch)
    {
        auto sound = GetSoundInstance(name);
        if (sound && m_platform)
        {
            float clampedPitch = (pitch < 0.1f) ? 0.1f : (pitch > 2.0f) ? 2.0f : pitch;
            m_platform->SetFrequencyRatio(sound->voice, clampedPitch);
        }
    }

    void AudioEngine::SetPan(const std::string& name, float pan)
    {
        auto sound = GetSoundInstance(name);
        if (sound)
        {
            ApplyPanning(*sound, pan);
        }
    }

    void AudioEngine::SetSoundPosition(const std::string& name, const Vector3& position)
    {
        auto sound = GetSoundInstance(name);
        if (!sound || !m_platform) return;

        sound->position = position;

        std::vector<float> matrix(sound->properties.channels * 2);
        float dopplerFactor = 1.0f;

        m_platform->CalculateSpatialParameters(sound->voice, position,
            matrix.data(), 2, &dopplerFactor);

        m_platform->SetOutputMatrix(sound->voice, matrix.data(),
            sound->properties.channels, 2);
        m_platform->SetFrequencyRatio(sound->voice, dopplerFactor);
    }

    void AudioEngine::SetListenerPosition(const Vector3& position, const Vector3& front, const Vector3& top)
    {
        if (m_initialized && m_platform)
        {
            m_platform->SetListenerPosition(position, front, top);
        }
    }

    void AudioEngine::ApplyFadeIn(const std::string& name, float duration)
    {
        auto sound = GetSoundInstance(name);
        if (!sound || !m_platform) return;

        sound->fadeTargetVolume = sound->currentVolume;
        sound->currentVolume = 0.0f;
        sound->fadeDuration = duration;
        sound->fadeTimer = 0.0f;
        sound->isFading = true;

        m_platform->SetVolume(sound->voice, 0.0f);
        PlaySound(name);
    }

    void AudioEngine::ApplyFadeOut(const std::string& name, float duration)
    {
        auto sound = GetSoundInstance(name);
        if (!sound) return;

        sound->fadeTargetVolume = 0.0f;
        sound->fadeDuration = duration;
        sound->fadeTimer = 0.0f;
        sound->isFading = true;
    }

    void AudioEngine::ApplyCrossFade(const std::string& from, const std::string& to, float duration)
    {
        ApplyFadeOut(from, duration);
        ApplyFadeIn(to, duration);
    }

    void AudioEngine::PlayLayer(const std::string& name, int layerId, bool loop)
    {
        auto soundData = m_sounds.find(name);
        if (soundData == m_sounds.end() || !m_platform) return;

        SoundInstance layerSound;
        layerSound.data = soundData->second.data;
        layerSound.properties = soundData->second.properties;

        if (!m_platform->CreateVoice(layerSound.properties, &layerSound.voice))
        {
            return;
        }

        m_platform->SubmitBuffer(layerSound.voice, layerSound.data.data(),
            static_cast<uint32_t>(layerSound.data.size()));

        layerSound.isPlaying = true;
        layerSound.currentVolume = 1.0f;

        m_platform->StartVoice(layerSound.voice);
        m_layers[layerId] = std::move(layerSound);
    }

    void AudioEngine::StopLayer(int layerId)
    {
        auto layer = GetLayerInstance(layerId);
        if (layer && m_platform)
        {
            m_platform->StopVoice(layer->voice);
            layer->isPlaying = false;
        }
    }

    void AudioEngine::SetLayerVolume(int layerId, float volume)
    {
        auto layer = GetLayerInstance(layerId);
        if (layer && m_platform)
        {
            layer->currentVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
            m_platform->SetVolume(layer->voice, layer->currentVolume);
        }
    }

    bool AudioEngine::IsPlaying(const std::string& name) const
    {
        auto sound = GetSoundInstance(name);
        return sound ? sound->isPlaying : false;
    }

    float AudioEngine::GetSoundDuration(const std::string& name) const
    {
        auto sound = GetSoundInstance(name);
        return sound ? sound->properties.duration : 0.0f;
    }

    void AudioEngine::UpdateFades(float deltaTime)
    {
        for (auto& pair : m_sounds)
        {
            auto& sound = pair.second;
            if (sound.isFading)
            {
                sound.fadeTimer += deltaTime;
                float t = sound.fadeTimer / sound.fadeDuration;
                if (t > 1.0f) t = 1.0f;

                float volume = sound.currentVolume + (sound.fadeTargetVolume - sound.currentVolume) * t;

                if (m_platform)
                {
                    m_platform->SetVolume(sound.voice, volume);
                }

                if (t >= 1.0f)
                {
                    sound.isFading = false;
                    sound.currentVolume = sound.fadeTargetVolume;

                    if (sound.fadeTargetVolume == 0.0f)
                    {
                        StopSound(pair.first);
                    }
                }
            }
        }
    }

    void AudioEngine::ApplyPanning(SoundInstance& sound, float pan)
    {
        if (!m_platform) return;

        if (sound.properties.channels == 2)
        {
            float clampedPan = (pan < -1.0f) ? -1.0f : (pan > 1.0f) ? 1.0f : pan;
            float left = 1.0f - ((clampedPan > 0.0f) ? clampedPan : 0.0f);
            float right = 1.0f + ((clampedPan < 0.0f) ? clampedPan : 0.0f);

            float matrix[4] = { left, 0.0f, 0.0f, right };
            m_platform->SetOutputMatrix(sound.voice, matrix, 2, 2);
        }
    }

    AudioEngine::SoundInstance* AudioEngine::GetSoundInstance(const std::string& name)
    {
        auto it = m_sounds.find(name);
        return it != m_sounds.end() ? &it->second : nullptr;
    }

    const AudioEngine::SoundInstance* AudioEngine::GetSoundInstance(const std::string& name) const
    {
        auto it = m_sounds.find(name);
        return it != m_sounds.end() ? &it->second : nullptr;
    }

    AudioEngine::SoundInstance* AudioEngine::GetLayerInstance(int layerId)
    {
        auto it = m_layers.find(layerId);
        return it != m_layers.end() ? &it->second : nullptr;
    }

    bool AudioEngine::LoadWAVFile(const std::string& filename, std::vector<uint8_t>& data, AudioProperties& properties)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;

        char header[12];
        file.read(header, 12);

        if (std::strncmp(header, "RIFF", 4) != 0 || std::strncmp(header + 8, "WAVE", 4) != 0)
            return false;

        while (file)
        {
            char chunkId[4];
            uint32_t chunkSize;

            file.read(chunkId, 4);
            if (!file) break;

            file.read(reinterpret_cast<char*>(&chunkSize), 4);
            if (!file) break;

            if (std::strncmp(chunkId, "fmt ", 4) == 0)
            {
                uint16_t formatTag, channels, blockAlign, bitsPerSample;
                uint32_t sampleRate, avgBytesPerSec;

                file.read(reinterpret_cast<char*>(&formatTag), 2);
                file.read(reinterpret_cast<char*>(&channels), 2);
                file.read(reinterpret_cast<char*>(&sampleRate), 4);
                file.read(reinterpret_cast<char*>(&avgBytesPerSec), 4);
                file.read(reinterpret_cast<char*>(&blockAlign), 2);
                file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

                properties.channels = channels;
                properties.sampleRate = sampleRate;
                properties.bitsPerSample = bitsPerSample;

                if (chunkSize > 16)
                {
                    file.seekg(chunkSize - 16, std::ios::cur);
                }
                break;
            }
            file.seekg(chunkSize, std::ios::cur);
        }

        while (file)
        {
            char chunkId[4];
            uint32_t chunkSize;

            file.read(chunkId, 4);
            if (!file) break;

            file.read(reinterpret_cast<char*>(&chunkSize), 4);
            if (!file) break;

            if (std::strncmp(chunkId, "data", 4) == 0)
            {
                data.resize(chunkSize);
                file.read(reinterpret_cast<char*>(data.data()), chunkSize);
                properties.dataSize = chunkSize;
                properties.duration = static_cast<float>(chunkSize) /
                    (properties.sampleRate * properties.channels * (properties.bitsPerSample / 8.0f));
                return true;
            }
            file.seekg(chunkSize, std::ios::cur);
        }

        return false;
    }
}