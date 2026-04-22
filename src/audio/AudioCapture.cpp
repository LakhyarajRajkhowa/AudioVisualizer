#define MINIAUDIO_IMPLEMENTATION
#include "audio/AudioCapture.h"
#include <iostream>
#include <cstring>

AudioCapture::AudioCapture()
{
    deviceConfig = ma_device_config_init(ma_device_type_playback);

    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 44100;

    deviceConfig.dataCallback = DataCallback;
    deviceConfig.pUserData = this;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
    {
        std::cout << "Failed to init device\n";
    }

    if (ma_device_start(&device) != MA_SUCCESS)
    {
        std::cout << "Failed to start device\n";
    }
}

AudioCapture::~AudioCapture()
{
    ma_device_uninit(&device);

    for (auto& [id, clip] : clips)
    {
        ma_decoder_uninit(&clip->decoder);
    }
}

bool AudioCapture::LoadAudio(int id, const std::string& filepath)
{
    auto clip = std::make_unique<AudioClip>();

    if (ma_decoder_init_file(filepath.c_str(), NULL, &clip->decoder) != MA_SUCCESS)
    {
        std::cout << "Failed to load audio\n";
        return false;
    }

    clip->filepath = filepath;
    clip->sampleRate = clip->decoder.outputSampleRate;
    clip->channels = clip->decoder.outputChannels;

    ma_decoder_get_length_in_pcm_frames(&clip->decoder, &clip->frameCount);

    //  Allocate visualization buffer (~2 sec)
    clip->visualBuffer.resize(clip->sampleRate * clip->channels * 2);

    clips[id] = std::move(clip);
    loadedAudios.insert(id);

    return true;
}

void AudioCapture::Play(int id)
{
    if (!clips.count(id)) return;

    for (auto& [otherID, clip] : clips)
    {
        clip->isPlaying = false;
    }

    currentPlayingID = id;

    auto& clip = clips[id];

    if (clip->currentFrame >= clip->frameCount)
    {
        ma_decoder_seek_to_pcm_frame(&clip->decoder, 0);
        clip->currentFrame = 0;
    }

    clip->isPlaying = true;
}

void AudioCapture::Pause(int id)
{
    if (clips.count(id))
    {
        clips[id]->isPlaying = false;
    }
}

void AudioCapture::Stop(int id)
{
    if (clips.count(id))
    {
        auto& clip = clips[id];
        clip->isPlaying = false;
        clip->currentFrame = 0;
        ma_decoder_seek_to_pcm_frame(&clip->decoder, 0);
    }
}

bool AudioCapture::IsPlaying(int id)
{
    if (!clips.count(id)) return false;
    return clips[id]->isPlaying;
}

void AudioCapture::SeekFrame(int id, uint64_t frame)
{
    if (clips.count(id))
    {
        auto& clip = clips[id];
        clip->needsSeek = true;
        clip->seekFrame = frame;
    }
}

uint64_t AudioCapture::GetPlaybackFrame(int id)
{
    if (clips.count(id))
        return clips[id]->currentFrame;
    return 0;
}

uint64_t AudioCapture::GetTotalFrames(int id)
{
    if (clips.count(id))
        return clips[id]->frameCount;
    return 0;
}

uint32_t AudioCapture::GetSampleRate(int id)
{
    if (clips.count(id))
        return clips[id]->sampleRate;
    return 0;
}

std::string AudioCapture::GetName(int id)
{
    if (clips.count(id))
        return clips[id]->filepath;
    return "";
}

std::vector<float> AudioCapture::GetSamplesWindow(int id, size_t fftSize)
{
    std::vector<float> result;

    if (!clips.count(id)) return result;

    auto& clip = clips[id];

    std::lock_guard<std::mutex> lock(clip->bufferMutex);

    size_t bufferSize = clip->visualBuffer.size();
    size_t start = (clip->writeCursor + bufferSize - fftSize) % bufferSize;

    result.resize(fftSize);

    for (size_t i = 0; i < fftSize; i++)
    {
        result[i] = clip->visualBuffer[(start + i) % bufferSize];
    }

    return result;
}

void AudioCapture::DataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount)
{
    auto* audio = (AudioCapture*)device->pUserData;
    float* out = (float*)output;

    if (audio->currentPlayingID == -1)
    {
        memset(out, 0, frameCount * 2 * sizeof(float));
        return;
    }

    auto& clip = audio->clips[audio->currentPlayingID];

    if (!clip->isPlaying)
    {
        memset(out, 0, frameCount * 2 * sizeof(float));
        return;
    }

    if (clip->needsSeek)
    {
        ma_decoder_seek_to_pcm_frame(&clip->decoder, clip->seekFrame);
        clip->currentFrame = clip->seekFrame;
        clip->needsSeek = false;
    }

    ma_uint32 channels = clip->channels;

    ma_uint64 framesRead = 0;

    ma_decoder_read_pcm_frames(
        &clip->decoder,
        out,
        frameCount,
        &framesRead
    );

    clip->currentFrame += framesRead;

    if (framesRead < frameCount)
    {
        memset(out + framesRead * channels, 0,
            (frameCount - framesRead) * channels * sizeof(float));

        clip->isPlaying = false;
    }

    {
        std::lock_guard<std::mutex> lock(clip->bufferMutex);

        size_t bufferSize = clip->visualBuffer.size();

        for (ma_uint32 i = 0; i < frameCount * 2; i++)
        {
            clip->visualBuffer[clip->writeCursor] = out[i];
            clip->writeCursor = (clip->writeCursor + 1) % bufferSize;
        }
    }
}