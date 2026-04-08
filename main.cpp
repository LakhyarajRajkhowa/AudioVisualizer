/*
    Lengine is the namespace of my game engine.
    Some files of Lengine are used in this project

*/


#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION


#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>

#include "audio/AudioCapture.h"
#include "audio/FFTProcessor.h"
#include "audio/AudioAnalyzer.h"
#include "audio/AudioManager.h"

#include "UI/Window.h"
#include "UI/ImguiLayer.h" 

#include "visualizer/RenderPipeline.h"

#include "external/tinyfiledialogs/tinyfiledialogs.h"


#define LIMIT_DELAY 16 // LIMIT_FPS = 1000 / LIMIT_DELAY

#define FFT_SIZE 1024

std::string rootFolderPath = "C:/Users/llakh/OneDrive/Desktop/Projects/AudioVisualizer/";

int main(int argc, char* argv[])
{
    bool running = true;

    AudioManager audioManager(rootFolderPath + "database/audiodb.json");

    AudioCapture audio;

    std::string path = rootFolderPath + "assets/audio/shape_of_you.mp3";
    std::string path2 = rootFolderPath + "assets/audio/One Direction - What Makes You Beautiful 7m.mp3";


    int id = audioManager.ImportAudio(path);
    int id2 = audioManager.ImportAudio(path2);

    audio.LoadAudio(id, path);
    audio.LoadAudio(id2, path2);





    Lengine::Window window("Audio Visualizer Test", 1280, 720, 0); 

    Lengine::ImGuiLayer imguiLayer(
        running,
        window.getWindow(),
        window.getGlContext(),
        audioManager.GetAudios(),
        audioManager.GetActiveAudios(),
        audio
    );

    Lengine::RenderPipeline renderPipeline;


    FFTProcessor fft(FFT_SIZE);
    AudioAnalyzer analyzer(FFT_SIZE);


    SDL_Event event;

    while (running)
    {
        // -------- Event Handling --------
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        imguiLayer.beginFrame();

        for (auto& id : audioManager.GetActiveAudios()) {


            auto sample_window = audio.GetSamplesWindow(id, FFT_SIZE);

            fft.Process(sample_window);

            const auto& spectrum = fft.GetSpectrum();

            analyzer.Analyze(spectrum);

            float bass = analyzer.GetBass();
            float mid = analyzer.GetMid();
            float treble = analyzer.GetTreble();

            const auto& smoothed = analyzer.GetSmoothedSpectrum();

            renderPipeline.Render(id, bass, mid, treble, smoothed, Lengine::VisualMode::MODE_1);

            imguiLayer.renderViewport(id, renderPipeline.GetFinalImage(id));


        }

        imguiLayer.renderPanels();
        imguiLayer.endFrame();


        SDL_Delay(LIMIT_DELAY);
        window.swapBuffer();
    }

    window.quitWindow();
    SDL_Quit();

    return 0;
}