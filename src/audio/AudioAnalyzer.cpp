#include "audio/AudioAnalyzer.h"

#include <algorithm>

AudioAnalyzer::AudioAnalyzer(size_t size)
{
    fftSize = size;

    smoothedSpectrum.resize(fftSize / 2, 0.0f);

    bass = 0.0f;
    mid = 0.0f;
    treble = 0.0f;

    smoothingFactor = 0.85f;
}

void AudioAnalyzer::Analyze(const std::vector<float>& spectrum)
{
    SmoothSpectrum(spectrum);
    ComputeBands();
}

void AudioAnalyzer::SmoothSpectrum(const std::vector<float>& spectrum)
{
    size_t count = std::min(smoothedSpectrum.size(), spectrum.size());

    for (size_t i = 0; i < count; i++)
    {
        smoothedSpectrum[i] =
            smoothedSpectrum[i] * smoothingFactor +
            spectrum[i] * (1.0f - smoothingFactor);
    }
}

void AudioAnalyzer::ComputeBands()
{
    bass = 0.0f;
    mid = 0.0f;
    treble = 0.0f;

    size_t n = smoothedSpectrum.size();

    size_t bassEnd = n * 0.10f;
    size_t midEnd = n * 0.40f;

    for (size_t i = 0; i < bassEnd; i++)
        bass += smoothedSpectrum[i];

    for (size_t i = bassEnd; i < midEnd; i++)
        mid += smoothedSpectrum[i];

    for (size_t i = midEnd; i < n; i++)
        treble += smoothedSpectrum[i];

    bass /= bassEnd + 1;
    mid /= (midEnd - bassEnd) + 1;
    treble /= (n - midEnd) + 1;
}

const std::vector<float>& AudioAnalyzer::GetSmoothedSpectrum() const
{
    return smoothedSpectrum;
}

float AudioAnalyzer::GetBass() const
{
    return bass;
}

float AudioAnalyzer::GetMid() const
{
    return mid;
}

float AudioAnalyzer::GetTreble() const
{
    return treble;
}