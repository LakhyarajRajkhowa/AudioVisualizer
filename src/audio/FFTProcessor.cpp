#include "audio/FFTProcessor.h"

#include <cmath>
#include <cstdlib>

#define M_PI 3.14159265358979323846

FFTProcessor::FFTProcessor(size_t size)
{
    fftSize = size;

    window.resize(fftSize);
    spectrum.resize(fftSize / 2);
    windowed.resize(fftSize);
    fftOutput.resize(fftSize / 2 + 1);

    GenerateHannWindow();

    cfg = kiss_fftr_alloc(fftSize, 0, nullptr, nullptr);
}

FFTProcessor::~FFTProcessor()
{
    free(cfg);
}

void FFTProcessor::GenerateHannWindow()
{
    for (size_t i = 0; i < fftSize; i++)
    {
        window[i] = 0.5f * (1.0f - cosf((2.0f * M_PI * i) / (fftSize - 1)));
    }
}

void FFTProcessor::Process(const std::vector<float>& samples)
{
    if (samples.size() < fftSize)
        return;

    // Apply window
    for (size_t i = 0; i < fftSize; i++)
    {
        windowed[i] = samples[i] * window[i];
    }

    // Run FFT
    kiss_fftr(cfg, windowed.data(), fftOutput.data());

    // Compute magnitudes
    for (size_t i = 0; i < fftSize / 2; i++)
    {
        float real = fftOutput[i].r;
        float imag = fftOutput[i].i;

        spectrum[i] = sqrtf(real * real + imag * imag);
    }
}

const std::vector<float>& FFTProcessor::GetSpectrum() const
{
    return spectrum;
}

size_t FFTProcessor::GetFFTSize() const
{
    return fftSize;
}