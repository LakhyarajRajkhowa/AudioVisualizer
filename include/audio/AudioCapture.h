#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>   // ✅ FIX
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <miniaudio/miniaudio.h>

struct AudioClip
{
    ma_decoder decoder{};
    ma_sound sound{};

    std::string filepath{};

    uint32_t sampleRate = 0;
    uint64_t frameCount = 0;
    uint32_t channels = 0;
};

class AudioCapture
{
public:
    AudioCapture();
    ~AudioCapture();

    AudioCapture(const AudioCapture&) = delete;
    AudioCapture& operator=(const AudioCapture&) = delete;

    bool LoadAudio(int id, const std::string& filepath);

    void Play(int id);
    void Pause(int id);
    void Stop(int id);

    std::string GetName(int id);
    uint64_t GetPlaybackFrame(int id);
    uint64_t GetTotalFrames(int id);
    uint32_t GetSampleRate(int id);

    void SeekFrame(int id, uint64_t frame);

    std::vector<float> GetSamplesWindow(int id, size_t fftSize);

    const std::unordered_set<int>& GetLoadedAudios() const { return loadedAudios; }

private:
    ma_engine engine{};  

    std::unordered_map<int, std::unique_ptr<AudioClip>> clips;
    std::unordered_set<int> loadedAudios;
};