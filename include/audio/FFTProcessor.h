#pragma once

#include <vector>
#include <cstddef>
#include <kiss_fftr.h>

class FFTProcessor
{
public:

    FFTProcessor(size_t fftSize);
    ~FFTProcessor();

    void Process(const std::vector<float>& samples);
    // Performs FFT on the first fftSize samples

    const std::vector<float>& GetSpectrum() const;
    // Returns frequency magnitude spectrum

    size_t GetFFTSize() const;
    // Returns FFT window size

private:

    size_t fftSize;

    std::vector<float> window;       // Hann window coefficients
    std::vector<float> spectrum;     // Output frequency magnitudes
    std::vector<float> windowed;     // Windowed input samples

    std::vector<kiss_fft_cpx> fftOutput; // Raw complex FFT output

    kiss_fftr_cfg cfg;               // KissFFT configuration (FFT plan)

    void GenerateHannWindow();
};