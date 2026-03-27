#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <miniaudio/miniaudio.h>

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

    uint64_t GetPlaybackFrame() const;
    // Returns current playback position in PCM frames

    std::vector<float> GetSamplesWindow(size_t fftSize) const;
    // Returns a mono FFT window starting at the current playback position

private:

    ma_engine engine;
    ma_decoder decoder;
    ma_sound sound;

    std::vector<float> samples;

    uint32_t sampleRate;
    uint64_t frameCount;
};