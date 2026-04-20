#pragma once

#include <vector>
#include <unordered_map>
#include <cstddef>

#define NUM_BARS 64

struct AnalyzerState
{
    std::vector<float> smoothedSpectrum;
    std::vector<float> logSpectrum;

    float bass = 0.0f;
    float mid = 0.0f;
    float treble = 0.0f;
};

class AudioAnalyzer
{
public:

    AudioAnalyzer(size_t fftSize);

    void Analyze(int id, const std::vector<float>& spectrum, const int sampleRate);

    const std::vector<float>& GetSmoothedSpectrum(int id);
    const std::vector<float>& GetLogSpectrum(int id);
    float GetBass(int id);
    float GetMid(int id);
    float GetTreble(int id);

private:

    void InitState(int id);
    void SmoothSpectrum(AnalyzerState& state, const std::vector<float>& spectrum);
    void ComputeLogSpectrum(AnalyzerState& state, const std::vector<float>& fft, const int sampleRate);
    void ComputeBands(AnalyzerState& state);

private:

    size_t fftSize;

    float smoothingFactor;

    std::unordered_map<int, AnalyzerState> states;
};