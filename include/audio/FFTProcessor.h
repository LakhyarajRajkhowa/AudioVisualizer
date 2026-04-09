#pragma once

#include <vector>
#include <unordered_map>
#include <cstddef>

#include <kissfft/kiss_fftr.h>

struct FFTState
{
    std::vector<float> window;
    std::vector<float> windowed;
    std::vector<float> spectrum;
    std::vector<kiss_fft_cpx> fftOutput;

    kiss_fftr_cfg cfg = nullptr;
};

class FFTProcessor
{
public:

    FFTProcessor(size_t fftSize);
    ~FFTProcessor();

    void Process(int id, const std::vector<float>& samples);

    const std::vector<float>& GetSpectrum(int id);

    size_t GetFFTSize() const;

private:

    size_t fftSize;

    std::unordered_map<int, FFTState> states;

    void InitState(int id);
    void GenerateHannWindow(std::vector<float>& window);
};