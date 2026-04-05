#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

#include <miniaudio/miniaudio.h>


struct AudioClip
{
    ma_decoder decoder;
    ma_sound sound;

    std::vector<float> samples;

    uint32_t sampleRate = 0;
    uint64_t frameCount = 0;
    uint32_t channels;
};

class AudioCapture
{
public:
    AudioCapture();
    ~AudioCapture();

    bool LoadAudio(int id, const std::string& filepath);

    void Play(int id);
    void Stop(int id);

    std::vector<float> GetSamplesWindow(int id, size_t fftSize);

    uint64_t GetPlaybackFrame(int id);

private:

    ma_engine engine;

    std::unordered_map<int, std::unique_ptr<AudioClip>> clips;



};