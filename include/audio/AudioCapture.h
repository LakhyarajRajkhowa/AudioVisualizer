#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <memory>

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

    void InitAudio(int id);
    void InitSamples(int id);
    void DestroySamples(int id);


    void Play(int id);
    void Pause(int id);
    void Stop(int id);

    std::string GetName(int id);
    std::vector<float> GetSamplesWindow(int id, size_t fftSize);

    uint64_t GetPlaybackFrame(int id);
    uint64_t GetTotalFrames(int id);
    uint32_t GetSampleRate(int id);

    void SeekFrame(int id, uint64_t frame);

    const std::unordered_set<int>& GetLoadedAudios() const { return loadedAudios; }



private:

    ma_engine engine;
    std::unordered_map<int, std::unique_ptr<AudioClip>> clips;
    std::unordered_map<int, bool> sampledAudio;
    std::unordered_set<int> loadedAudios;


};