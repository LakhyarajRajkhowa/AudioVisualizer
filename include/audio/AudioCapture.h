#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <miniaudio.h>

class AudioCapture
{
public:
    AudioCapture();
    ~AudioCapture();

    bool LoadFile(const std::string& filepath);
    void Play();
    void Stop();

    const std::vector<float>& GetSamples() const;

    uint32_t GetSampleRate() const;
    uint64_t GetFrameCount() const;

private:

    ma_engine engine;
    ma_sound sound;

    ma_decoder decoder;


    std::vector<float> samples;

    uint32_t sampleRate;

    uint64_t frameCount;
};

// ----- std::vector<float> samples ----

// Raw decoded PCM audio samples extracted from the audio file.
//
// Each value represents the amplitude of the sound wave at a specific time.
// The values are normalized floating-point numbers in the range [-1.0, 1.0].
//
// Example:
// 0.25   → positive pressure
// -0.40  → negative pressure
// 0.0    → silence
//
// If the audio is stereo (2 channels), the data layout is interleaved:
//
// L R L R L R L R ...
//
// Example:
// samples[0] → Left channel sample
// samples[1] → Right channel sample
// samples[2] → Next left sample
//
// These samples represent the audio signal in the **time domain** and will
// later be processed in small windows using FFT to obtain the frequency spectrum
// used for visualization.

// ----- uint32_t sampleRate -----

// The sample rate of the decoded audio file (in Hertz).
//
// This tells how many audio samples exist per second.
//
// Example:
// 44100 Hz → 44100 samples per second
// 48000 Hz → 48000 samples per second
//
// This value is important because it defines the relationship between
// sample index and time:
//
// time (seconds) = sampleIndex / sampleRate
//
// Example:
// sampleIndex = 44100  → 1 second of audio (if sampleRate = 44100)

// ----- uint64_t frameCount -----

// Total number of PCM frames in the decoded audio.
//
// A frame represents **one sample for each channel**.
//
// Example:
//
// Mono audio (1 channel)
// frame 0 → sample[0]
//
// Stereo audio (2 channels)
// frame 0 → sample[0] = Left
//            sample[1] = Right
//
// The total number of samples stored in the vector is:
//
// samples.size() = frameCount × channelCount
//
// This value allows us to compute the total duration of the audio:
//
// duration(seconds) = frameCount / sampleRate
//
// Example:
// frameCount = 4,410,000
// sampleRate = 44,100
// duration ≈ 100 seconds