#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>

#include <miniaudio/miniaudio.h>

struct AudioClip
{
    ma_decoder decoder{};
    std::string filepath{};

    uint32_t sampleRate = 0;
    ma_uint64 frameCount = 0;
    uint32_t channels = 0;

    ma_uint64 currentFrame = 0;
    bool isPlaying = false;
    bool needsSeek = false;
    ma_uint64 seekFrame = 0;

    //  Visualization buffer (circular)
    std::vector<float> visualBuffer;
    size_t writeCursor = 0;

    std::mutex bufferMutex;
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

    bool IsPlaying(int id);

private:
    static void DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);

    ma_device device{};
    ma_device_config deviceConfig{};

    std::unordered_map<int, std::unique_ptr<AudioClip>> clips;
    std::unordered_set<int> loadedAudios;

    int currentPlayingID = -1;
};