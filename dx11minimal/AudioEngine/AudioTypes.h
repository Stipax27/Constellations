#pragma once
#include <cstdint>
#include <array>
#include <algorithm>

#ifdef _WIN32
#include <xaudio2.h>
#include <x3daudio.h>
#endif

namespace Audio
{
    struct Vector3
    {
        float x, y, z;
        Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    };

    enum class AudioResult
    {
        Success,
        Error_InitFailed,
        Error_FileNotFound,
        Error_InvalidFormat,
        Error_DeviceLost,
        Error_PlatformNotSupported
    };

    struct AudioProperties
    {
        uint32_t sampleRate;
        uint16_t bitsPerSample;
        uint16_t channels;
        uint32_t dataSize;
        float duration;

        AudioProperties() : sampleRate(0), bitsPerSample(0), channels(0), dataSize(0), duration(0.0f) {}
    };
}