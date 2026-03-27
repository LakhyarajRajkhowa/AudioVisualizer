#pragma once

#include <vector>
#include <cstddef>

class AudioAnalyzer
{
public:

    AudioAnalyzer(size_t fftSize);

    void Analyze(const std::vector<float>& spectrum);

    const std::vector<float>& GetSmoothedSpectrum() const;

    float GetBass() const;
    float GetMid() const;
    float GetTreble() const;

private:

    void SmoothSpectrum(const std::vector<float>& spectrum);
    void ComputeBands();

private:

    size_t fftSize;

    std::vector<float> smoothedSpectrum;

    float bass;
    float mid;
    float treble;

    float smoothingFactor;
};