/*
    Lengine is the namespace of my game engine.
    Some files of Lengine are used in this project

*/


#define SDL_MAIN_HANDLED

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>

#include "audio/AudioCapture.h"
#include "audio/FFTProcessor.h"
#include "audio/AudioAnalyzer.h"
#include "UI/Window.h"
#include "UI/ImguiLayer.h"

#define LIMIT_DELAY 16 // LIMIT_FPS = 1000 / LIMIT_DELAY

#define FFT_SIZE 1024


int main(int argc, char* argv[])
{
    bool running = true;

    AudioCapture audio;
    audio.LoadFile("../assets/audio/shape_of_you.mp3");
    audio.Play();

    Lengine::Window window("Audio Visualizer Test", 1280, 720, 0); 

    Lengine::ImGuiLayer imguiLayer(
        running,
        window.getWindow(),
        window.getGlContext()
    );


    FFTProcessor fft(FFT_SIZE);
    AudioAnalyzer analyzer(FFT_SIZE);


    SDL_Event event;

    while (running)
    {
        // -------- Event Handling --------
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        imguiLayer.beginFrame();


        auto sample_window = audio.GetSamplesWindow(FFT_SIZE);

        fft.Process(sample_window);

        const auto& spectrum = fft.GetSpectrum();

        analyzer.Analyze(spectrum);

        float bass = analyzer.GetBass();
        float mid = analyzer.GetMid();
        float treble = analyzer.GetTreble();

        const auto& smoothed = analyzer.GetSmoothedSpectrum();

        // (Red, Green, Blue, Alpha)
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_Delay(LIMIT_DELAY); 

        imguiLayer.endFrame();
        window.swapBuffer();
    }

    window.quitWindow();
    SDL_Quit();

    return 0;
}