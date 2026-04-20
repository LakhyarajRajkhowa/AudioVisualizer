#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "audio/AudioCapture.h"

AudioCapture::AudioCapture()
{
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
    {
        std::cout << "Failed to initialize audio engine\n";
    }
}

AudioCapture::~AudioCapture()
{
    for (auto& [id, clip] : clips)
    {
        ma_sound_uninit(&clip->sound);
        ma_decoder_uninit(&clip->decoder);
    }

    ma_engine_uninit(&engine);
}

bool AudioCapture::LoadAudio(int id, const std::string& filepath)
{
    if (loadedAudios.find(id) != loadedAudios.end())
        return true;

    auto clip = std::make_unique<AudioClip>();
    clip->filepath = filepath;

    ma_decoder_config config = ma_decoder_config_init_default();
    config.format = ma_format_f32;

    if (ma_decoder_init_file(filepath.c_str(), &config, &clip->decoder) != MA_SUCCESS)
    {
        std::cout << "Failed to load audio\n";
        return false;
    }

    if (ma_sound_init_from_file(
        &engine,
        filepath.c_str(),
        MA_SOUND_FLAG_STREAM,
        NULL,
        NULL,
        &clip->sound) != MA_SUCCESS)
    {
        std::cout << "Failed to initialize sound\n";
        return false;
    }

    clip->sampleRate = clip->decoder.outputSampleRate;
    clip->channels = clip->decoder.outputChannels;

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&clip->decoder, &totalFrames);
    clip->frameCount = totalFrames;

    // 🔥 important for sync fix
    clip->lastFrame = 0;
    clip->needsSeek = true;

    clips[id] = std::move(clip);
    loadedAudios.insert(id);

    return true;
}

void AudioCapture::Play(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return;

    ma_sound_start(&it->second->sound);
}

void AudioCapture::Pause(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return;

    ma_sound_stop(&it->second->sound);
}

void AudioCapture::Stop(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return;

    ma_sound_stop(&it->second->sound);
    ma_sound_seek_to_pcm_frame(&it->second->sound, 0);

    // reset decoder sync
    it->second->lastFrame = 0;
    it->second->needsSeek = true;
}

uint64_t AudioCapture::GetPlaybackFrame(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return 0;

    ma_uint64 cursor = 0;
    ma_sound_get_cursor_in_pcm_frames(&it->second->sound, &cursor);

    return cursor;
}

uint64_t AudioCapture::GetTotalFrames(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return 0;

    return it->second->frameCount;
}

uint32_t AudioCapture::GetSampleRate(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return 0;

    return it->second->sampleRate;
}

void AudioCapture::SeekFrame(int id, uint64_t frame)
{
    auto it = clips.find(id);
    if (it == clips.end()) return;

    ma_sound_seek_to_pcm_frame(&it->second->sound, frame);

    it->second->lastFrame = frame;
    it->second->needsSeek = true;
}

std::string AudioCapture::GetName(int id)
{
    auto it = clips.find(id);
    if (it == clips.end()) return "";

    return it->second->filepath;
}

std::vector<float> AudioCapture::GetSamplesWindow(int id, size_t fftSize)
{
    std::vector<float> window(fftSize, 0.0f);

    auto it = clips.find(id);
    if (it == clips.end())
    {
        std::cout << "Audio ID not found\n";
        return window;
    }

    AudioClip& clip = *it->second;

    if (clip.channels == 0)
        return window;

    ma_uint64 currentFrame = GetPlaybackFrame(id);

    if (currentFrame >= clip.frameCount)
        currentFrame = clip.frameCount - 1;

    // 🔥 ONLY seek decoder when needed
    if (clip.needsSeek)
    {
        ma_decoder_seek_to_pcm_frame(&clip.decoder, currentFrame);
        clip.lastFrame = currentFrame;
        clip.needsSeek = false;
    }

    std::vector<float> tempBuffer(fftSize * clip.channels);

    ma_uint64 framesRead = 0;

    ma_decoder_read_pcm_frames(
        &clip.decoder,
        tempBuffer.data(),
        fftSize,
        &framesRead
    );

    clip.lastFrame += framesRead;

    // convert to mono
    for (size_t i = 0; i < framesRead; i++)
    {
        float left = tempBuffer[i * clip.channels];
        float right = (clip.channels > 1)
            ? tempBuffer[i * clip.channels + 1]
            : left;

        window[i] = (left + right) * 0.5f;
    }

    return window;
}