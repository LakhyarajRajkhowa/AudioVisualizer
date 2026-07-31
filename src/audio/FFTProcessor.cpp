#include "audio/FFTProcessor.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

#define M_PI 3.14159265358979323846


FFTProcessor::FFTProcessor(size_t size)
{
    fftSize = size;
}

FFTProcessor::~FFTProcessor()
{
    for (auto& [id, state] : states)
    {
        if (state.cfg)
            free(state.cfg);
    }
}


void FFTProcessor::GenerateHannWindow(std::vector<float>& window)
{
    for (size_t i = 0; i < fftSize; i++)
    {
        window[i] = 0.5f * (1.0f - cosf((2.0f * M_PI * i) / (fftSize - 1)));
    }
}


void FFTProcessor::InitState(int id)
{
    FFTState state;

    state.window.resize(fftSize);
    state.windowed.resize(fftSize);
    state.spectrum.resize(fftSize / 2);
    state.fftOutput.resize(fftSize / 2 + 1);

    GenerateHannWindow(state.window);

    state.cfg = kiss_fftr_alloc(fftSize, 0, nullptr, nullptr);

    states[id] = std::move(state);
}


void FFTProcessor::Process(int id, const std::vector<float>& samples)
{
    if (samples.size() < fftSize)
        return;

    if (!states.count(id))
        InitState(id);

    FFTState& state = states[id];

    for (size_t i = 0; i < fftSize; i++)
    {
        state.windowed[i] = samples[i] * state.window[i];
    }

    kiss_fftr(state.cfg, state.windowed.data(), state.fftOutput.data());

    for (size_t i = 0; i < fftSize / 2; i++)
    {
        float real = state.fftOutput[i].r;
        float imag = state.fftOutput[i].i;

        state.spectrum[i] = sqrtf(real * real + imag * imag);
    }
}


const std::vector<float>& FFTProcessor::GetSpectrum(int id)
{
    return states[id].spectrum;
}


size_t FFTProcessor::GetFFTSize() const
{
    return fftSize;
}