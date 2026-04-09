#include "audio/AudioAnalyzer.h"

#include <algorithm>
#include <iostream>

AudioAnalyzer::AudioAnalyzer(size_t size)
{
    fftSize = size;
    smoothingFactor = 0.85f;
}


void AudioAnalyzer::InitState(int id)
{
    AnalyzerState state;

    state.smoothedSpectrum.resize(fftSize / 2, 0.0f);

    states[id] = std::move(state);
}


void AudioAnalyzer::Analyze(int id, const std::vector<float>& spectrum)
{
    if (!states.count(id))
        InitState(id);

    AnalyzerState& state = states[id];

    SmoothSpectrum(state, spectrum);
    ComputeBands(state);
}


void AudioAnalyzer::SmoothSpectrum(AnalyzerState& state, const std::vector<float>& spectrum)
{
    size_t count = std::min(state.smoothedSpectrum.size(), spectrum.size());

    for (size_t i = 0; i < count; i++)
    {
        state.smoothedSpectrum[i] =
            state.smoothedSpectrum[i] * smoothingFactor +
            spectrum[i] * (1.0f - smoothingFactor);
    }
}


void AudioAnalyzer::ComputeBands(AnalyzerState& state)
{
    state.bass = 0.0f;
    state.mid = 0.0f;
    state.treble = 0.0f;

    size_t n = state.smoothedSpectrum.size();

    size_t bassEnd = n * 0.10f;
    size_t midEnd = n * 0.40f;

    for (size_t i = 0; i < bassEnd; i++)
        state.bass += state.smoothedSpectrum[i];

    for (size_t i = bassEnd; i < midEnd; i++)
        state.mid += state.smoothedSpectrum[i];

    for (size_t i = midEnd; i < n; i++)
        state.treble += state.smoothedSpectrum[i];

    state.bass /= bassEnd + 1;
    state.mid /= (midEnd - bassEnd) + 1;
    state.treble /= (n - midEnd) + 1;
}


const std::vector<float>& AudioAnalyzer::GetSmoothedSpectrum(int id)
{
    return states[id].smoothedSpectrum;
}


float AudioAnalyzer::GetBass(int id)
{
    return states[id].bass;
}

float AudioAnalyzer::GetMid(int id)
{
    return states[id].mid;
}

float AudioAnalyzer::GetTreble(int id)
{
    return states[id].treble;
}