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
    auto clip = std::make_unique<AudioClip>();

    ma_decoder_config config = ma_decoder_config_init(
        ma_format_f32,
        2,
        44100
    );

    if (ma_decoder_init_file(filepath.c_str(), &config, &clip->decoder) != MA_SUCCESS)
    {
        std::cout << "Failed to load audio\n";
        return false;
    }

  

    if (ma_sound_init_from_file(
        &engine,
        filepath.c_str(),
        0,
        NULL,
        NULL,
        &clip->sound) != MA_SUCCESS)
    {
        std::cout << "Failed to initialize sound\n";
        return false;
    }

    clips[id] = std::move(clip);

    return true;
}
void AudioCapture::InitAudio(int id)
{
    auto& clip = clips[id];

    clip->sampleRate = clip->decoder.outputSampleRate;
    clip->channels = clip->decoder.outputChannels;

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&clip->decoder, &totalFrames);

    clip->frameCount = totalFrames;

    
}

void AudioCapture::InitSamples(int id)
{
    auto& clip = clips[id];

    clip->samples.resize(clip->frameCount * clip->channels);

    ma_decoder_read_pcm_frames(
        &clip->decoder,
        clip->samples.data(),
        clip->frameCount,
        NULL
    );
   
}

void AudioCapture::Play(int id)
{
    ma_sound_start(&clips[id]->sound);
}

void AudioCapture::Pause(int id)
{
    ma_sound_stop(&clips[id]->sound);
}

void AudioCapture::Stop(int id)
{
    ma_sound_stop(&clips[id]->sound);
    ma_sound_seek_to_pcm_frame(&clips[id]->sound, 0); 
}


uint64_t AudioCapture::GetPlaybackFrame(int id)
{
    ma_uint64 cursor = 0;
    ma_sound_get_cursor_in_pcm_frames(&clips[id]->sound, &cursor);
    return cursor;
}

uint64_t AudioCapture::GetTotalFrames(int id)
{
   
    return clips[id]->frameCount;
}

uint32_t AudioCapture::GetSampleRate(int id)
{
   
    return clips[id]->sampleRate;
}

void AudioCapture::SeekFrame(int id, uint64_t frame)
{
    ma_sound_seek_to_pcm_frame(&clips[id]->sound, frame);
}

std::vector<float> AudioCapture::GetSamplesWindow(int id, size_t fftSize)
{
    if (!sampledAudio[id]) {
        InitSamples(id);
        
        sampledAudio[id] = true;
    }
    std::vector<float> window(fftSize);

    auto it = clips.find(id);

    if (it == clips.end())
    {
        std::cout << "Audio ID not found\n";
        return window;
    }

    AudioClip& clip = *it->second;

    uint64_t frame = GetPlaybackFrame(id);

    if (frame >= clip.frameCount)
        frame = clip.frameCount - 1;

    for (size_t i = 0; i < fftSize; i++)
    {
        uint64_t frameIndex = frame + i;

        if (frameIndex >= clip.frameCount)
        {
            window[i] = 0.0f;
            continue;
        }

        size_t sampleIndex = frameIndex * clip.channels;

        float left = clip.samples[sampleIndex];

        float right = left;

        if (clip.channels > 1)
            right = clip.samples[sampleIndex + 1];

        window[i] = (left + right) * 0.5f;
    }

    return window;
}