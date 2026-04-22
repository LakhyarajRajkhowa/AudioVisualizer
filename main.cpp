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

#include "utils/Timer.h"


#define LIMIT_DELAY 16 // LIMIT_FPS = 1000 / LIMIT_DELAY

#define FFT_SIZE 1024

#define SAMPLE_RATE 44100


/*
   !!!IMPORTANT  : Enter your root folder path here 
*/

std::string rootFolderPath = "C:/Users/llakh/OneDrive/Desktop/Projects/AudioVisualizer/";


int main(int argc, char* argv[])
{

    bool running = true;

    Timer timer;

    AudioManager audioManager(rootFolderPath + "database/audiodb.json");
    ResourceManager resouceManager;

    AudioCapture audio;

    Lengine::Window window("Audio Visualizer Test", 1280, 720, 0); 

    Lengine::RenderPipeline renderPipeline(resouceManager);


    Lengine::ImGuiLayer imguiLayer(
        running,
        window.getWindow(),
        window.getGlContext(),
        audioManager,
        audioManager.GetAudios(),
        audio,
        renderPipeline
    );

    FFTProcessor fft(FFT_SIZE);
    AudioAnalyzer analyzer(FFT_SIZE);


    SDL_Event event;

    while (running)
    {
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

            fft.Process(id, sample_window);

            const auto& spectrum = fft.GetSpectrum(id);

            analyzer.Analyze(id, spectrum, SAMPLE_RATE);

            float bass = analyzer.GetBass(id);
            float mid = analyzer.GetMid(id);
            float treble = analyzer.GetTreble(id);

            const auto& smoothedSpectrum = analyzer.GetSmoothedSpectrum(id);
            const auto& logSpectrum = analyzer.GetLogSpectrum(id);


            auto& renderContexts = renderPipeline.GetRenderContexts();

            
            renderPipeline.Render(
                id,
                bass,
                mid,
                treble,
                spectrum,
                smoothedSpectrum,
                logSpectrum,
                renderPipeline.renderModes[id],
                timer.GetTime());

            imguiLayer.renderViewport(id, renderPipeline.GetFinalImage(id), renderPipeline.renderModes[id]);

        }

        audioManager.Update(renderPipeline);

        imguiLayer.renderPanels();
        imguiLayer.endFrame();


        SDL_Delay(LIMIT_DELAY);
        window.swapBuffer();
    }

    window.quitWindow();
    SDL_Quit();

    return 0;
}