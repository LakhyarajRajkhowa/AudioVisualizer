#include "audio/AudioAnalyzer.h"

#include <algorithm>
#include <iostream>


void AudioAnalyzer::ComputeLogSpectrum(
    AnalyzerState& state,
    const std::vector<float>& fft,
    const int sampleRate
    )
{
    int fftSize = fft.size();
    int numBars = state.logSpectrum.size();

    float minFreq = 20.0f;
    float maxFreq = sampleRate * 0.5f;

    for (int i = 0; i < numBars; i++)
    {
        float t0 = (float)i / numBars;
        float t1 = (float)(i + 1) / numBars;

        float f0 = minFreq * pow(maxFreq / minFreq, t0);
        float f1 = minFreq * pow(maxFreq / minFreq, t1);

        int bin0 = (int)(f0 / maxFreq * fftSize);
        int bin1 = (int)(f1 / maxFreq * fftSize);

        bin0 = std::clamp(bin0, 0, fftSize - 1);
        bin1 = std::clamp(bin1, 0, fftSize - 1);

        float sum = 0.0f;
        int count = 0;

        for (int j = bin0; j <= bin1; j++)
        {
            float v = log(1.0f + fft[j] * 10.0f);
            sum += v;
            count++;
        }

        float value = (count > 0) ? sum / count : 0.0f;

        float weight = 1.0f + 2.5f * (1.0f - t0);
        value *= weight;

        state.logSpectrum[i] =
            state.logSpectrum[i] * smoothingFactor +
            value * (1.0f - smoothingFactor);
    }
}
AudioAnalyzer::AudioAnalyzer(size_t size)
{
    fftSize = size;
    smoothingFactor = 0.85f;
}


void AudioAnalyzer::InitState(int id)
{
    AnalyzerState state;

    state.smoothedSpectrum.resize(fftSize / 2, 0.0f);

    state.logSpectrum.resize(NUM_BARS, 0.0f);

    states[id] = std::move(state);
}

void AudioAnalyzer::Analyze(int id, const std::vector<float>& spectrum, const int sampleRate)
{
    if (!states.count(id))
        InitState(id);

    AnalyzerState& state = states[id];


    SmoothSpectrum(state, spectrum);
    ComputeBands(state);

    ComputeLogSpectrum(state, spectrum, sampleRate);

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

const std::vector<float>& AudioAnalyzer::GetLogSpectrum(int id)
{
    return states[id].logSpectrum;
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