#define MINIAUDIO_IMPLEMENTATION
#include "audio/AudioCapture.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>

AudioCapture::AudioCapture()
{
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 44100;
    deviceConfig.dataCallback = DataCallback;
    deviceConfig.pUserData = this;

    const ma_result initResult = ma_device_init(nullptr, &deviceConfig, &device);
    if (initResult != MA_SUCCESS)
    {
        std::cerr << "[Audio Error] Could not initialize playback device (miniaudio code "
                  << initResult << "). Audio playback is unavailable.\n";
        return;
    }

    deviceInitialized = true;

    const ma_result startResult = ma_device_start(&device);
    if (startResult != MA_SUCCESS)
    {
        std::cerr << "[Audio Error] Could not start playback device (miniaudio code "
                  << startResult << ").\n";
        ma_device_uninit(&device);
        deviceInitialized = false;
    }
}

AudioCapture::~AudioCapture()
{
    if (deviceInitialized)
    {
        ma_device_stop(&device);
        ma_device_uninit(&device);
        deviceInitialized = false;
    }

    std::lock_guard<std::mutex> lock(clipsMutex);
    for (auto& [id, clip] : clips)
        ma_decoder_uninit(&clip->decoder);
}

bool AudioCapture::LoadAudio(int id, const std::string& filepath)
{
    if (id < 0 || filepath.empty())
    {
        std::cerr << "[Audio Error] Invalid audio ID or empty file path.\n";
        return false;
    }

    std::error_code ec;
    const std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path, ec) || ec ||
        !std::filesystem::is_regular_file(path, ec) || ec)
    {
        std::cerr << "[Audio Error] File does not exist or is not a regular file: "
                  << filepath << '\n';
        return false;
    }

    auto clip = std::make_unique<AudioClip>();
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 44100);
    const ma_result result = ma_decoder_init_file(filepath.c_str(), &decoderConfig, &clip->decoder);
    if (result != MA_SUCCESS)
    {
        std::cerr << "[Audio Error] Could not decode '" << filepath
                  << "' (miniaudio code " << result
                  << "). The file may be corrupt or unsupported.\n";
        return false;
    }

    clip->filepath = filepath;
    clip->sampleRate = clip->decoder.outputSampleRate;
    clip->channels = clip->decoder.outputChannels;

    if (ma_decoder_get_length_in_pcm_frames(&clip->decoder, &clip->frameCount) != MA_SUCCESS ||
        clip->sampleRate == 0 || clip->channels == 0)
    {
        std::cerr << "[Audio Error] Could not read audio metadata: " << filepath << '\n';
        ma_decoder_uninit(&clip->decoder);
        return false;
    }

    // Two seconds of stereo samples for visualization.
    clip->visualBuffer.resize(static_cast<size_t>(clip->sampleRate) * clip->channels * 2, 0.0f);

    std::lock_guard<std::mutex> lock(clipsMutex);
    auto old = clips.find(id);
    if (old != clips.end())
        ma_decoder_uninit(&old->second->decoder);

    clips[id] = std::move(clip);
    loadedAudios.insert(id);
    return true;
}

void AudioCapture::Play(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    if (it == clips.end() || !deviceInitialized)
    {
        std::cerr << "[Audio Error] Cannot play audio ID " << id
                  << ": it is not loaded or the playback device is unavailable.\n";
        return;
    }

    for (auto& [otherID, clip] : clips)
        clip->isPlaying = false;

    currentPlayingID = id;
    AudioClip& clip = *it->second;
    if (clip.currentFrame >= clip.frameCount)
    {
        ma_decoder_seek_to_pcm_frame(&clip.decoder, 0);
        clip.currentFrame = 0;
    }
    clip.isPlaying = true;
}

void AudioCapture::Pause(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    if (it != clips.end())
        it->second->isPlaying = false;
}

void AudioCapture::Stop(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    if (it == clips.end()) return;

    AudioClip& clip = *it->second;
    clip.isPlaying = false;
    clip.currentFrame = 0;
    ma_decoder_seek_to_pcm_frame(&clip.decoder, 0);
}

bool AudioCapture::IsPlaying(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    return it != clips.end() && it->second->isPlaying;
}

void AudioCapture::SeekFrame(int id, uint64_t frame)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    if (it != clips.end())
    {
        it->second->needsSeek = true;
        it->second->seekFrame = std::min<ma_uint64>(frame, it->second->frameCount);
    }
}

uint64_t AudioCapture::GetPlaybackFrame(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    return it == clips.end() ? 0 : it->second->currentFrame;
}

uint64_t AudioCapture::GetTotalFrames(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    return it == clips.end() ? 0 : it->second->frameCount;
}

uint32_t AudioCapture::GetSampleRate(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    return it == clips.end() ? 0 : it->second->sampleRate;
}

std::string AudioCapture::GetName(int id)
{
    std::lock_guard<std::mutex> lock(clipsMutex);
    auto it = clips.find(id);
    return it == clips.end() ? std::string{} : it->second->filepath;
}

std::vector<float> AudioCapture::GetSamplesWindow(int id, size_t fftSize)
{
    std::lock_guard<std::mutex> clipsLock(clipsMutex);
    auto it = clips.find(id);
    if (it == clips.end()) return {};

    AudioClip& clip = *it->second;
    std::lock_guard<std::mutex> bufferLock(clip.bufferMutex);
    const size_t bufferSize = clip.visualBuffer.size();
    if (bufferSize == 0 || fftSize == 0 || fftSize > bufferSize)
        return {};

    const size_t start = (clip.writeCursor + bufferSize - fftSize) % bufferSize;
    std::vector<float> result(fftSize);
    for (size_t i = 0; i < fftSize; ++i)
        result[i] = clip.visualBuffer[(start + i) % bufferSize];

    return result;
}

void AudioCapture::DataCallback(ma_device* device, void* output, const void*, ma_uint32 frameCount)
{
    auto* audio = static_cast<AudioCapture*>(device->pUserData);
    auto* out = static_cast<float*>(output);
    constexpr ma_uint32 outputChannels = 2;

    std::memset(out, 0, static_cast<size_t>(frameCount) * outputChannels * sizeof(float));

    // Synchronization prevents the UI thread from replacing a clip while the
    // real-time callback is decoding it. Keep this critical section minimal.
    std::lock_guard<std::mutex> clipsLock(audio->clipsMutex);
    auto it = audio->clips.find(audio->currentPlayingID);
    if (it == audio->clips.end() || !it->second->isPlaying)
        return;

    AudioClip& clip = *it->second;
    if (clip.needsSeek)
    {
        const ma_uint64 target = std::min<ma_uint64>(clip.seekFrame, clip.frameCount);
        if (ma_decoder_seek_to_pcm_frame(&clip.decoder, target) == MA_SUCCESS)
            clip.currentFrame = target;
        clip.needsSeek = false;
    }

    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(&clip.decoder, out, frameCount, &framesRead);
    clip.currentFrame += framesRead;

    if (framesRead < frameCount)
        clip.isPlaying = false;

    std::lock_guard<std::mutex> bufferLock(clip.bufferMutex);
    const size_t bufferSize = clip.visualBuffer.size();
    if (bufferSize == 0) return;

    const size_t sampleCount = static_cast<size_t>(frameCount) * outputChannels;
    for (size_t i = 0; i < sampleCount; ++i)
    {
        clip.visualBuffer[clip.writeCursor] = out[i];
        clip.writeCursor = (clip.writeCursor + 1) % bufferSize;
    }
}
