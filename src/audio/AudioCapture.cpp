#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "audio/AudioCapture.h"

AudioCapture::AudioCapture()
{
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
    {
        std::cout << "Failed to initialize audio engine\n";
    }

    sampleRate = 0;
    frameCount = 0;
}

AudioCapture::~AudioCapture()
{
    ma_sound_uninit(&sound);
    ma_decoder_uninit(&decoder);
    ma_engine_uninit(&engine);
}

bool AudioCapture::LoadFile(const std::string& filepath)
{
    ma_result result;

    ma_decoder_config config = ma_decoder_config_init(
        ma_format_f32,
        2,
        44100
    );

    result = ma_decoder_init_file(filepath.c_str(), &config, &decoder);

    if (result != MA_SUCCESS)
    {
        std::cout << "Failed to load audio file\n";
        return false;
    }

    sampleRate = decoder.outputSampleRate;

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);

    frameCount = totalFrames;

    uint32_t channels = decoder.outputChannels;

    samples.resize(frameCount * channels);

    result = ma_decoder_read_pcm_frames(
        &decoder,
        samples.data(),
        frameCount,
        NULL
    );

    if (result != MA_SUCCESS)
    {
        std::cout << "Failed to read PCM frames\n";
        return false;
    }

    result = ma_sound_init_from_file(
        &engine,
        filepath.c_str(),
        0,
        NULL,
        NULL,
        &sound
    );

    if (result != MA_SUCCESS)
    {
        std::cout << "Failed to initialize sound\n";
        return false;
    }

    std::cout << "Audio Loaded\n";
    std::cout << "Sample Rate: " << sampleRate << "\n";
    std::cout << "Frames: " << frameCount << "\n";

    return true;
}

void AudioCapture::Play()
{
    ma_sound_start(&sound);
}

void AudioCapture::Stop()
{
    ma_sound_stop(&sound);
}

const std::vector<float>& AudioCapture::GetSamples() const
{
    return samples;
}

uint32_t AudioCapture::GetSampleRate() const
{
    return sampleRate;
}

uint64_t AudioCapture::GetFrameCount() const
{
    return frameCount;
}

uint64_t AudioCapture::GetPlaybackFrame() const
{
    ma_uint64 cursor = 0;
    ma_sound_get_cursor_in_pcm_frames(&sound, &cursor);
    return cursor;
}

std::vector<float> AudioCapture::GetSamplesWindow(size_t fftSize) const
{
    std::vector<float> window(fftSize);

    uint64_t frame = GetPlaybackFrame();

    uint32_t channels = decoder.outputChannels;

    for (size_t i = 0; i < fftSize; i++)
    {
        uint64_t frameIndex = frame + i;

        if (frameIndex >= frameCount)
        {
            window[i] = 0.0f;
            continue;
        }

        size_t sampleIndex = frameIndex * channels;

        float left = samples[sampleIndex];
        float right = samples[sampleIndex + 1];

        window[i] = (left + right) * 0.5f;
    }

    return window;
}