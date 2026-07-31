/*
    Lengine is the namespace of my game engine.
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

#include "tinyfiledialogs/tinyfiledialogs.h"

#include "utils/Timer.h"

#include "network/NetworkService.h"

#define LIMIT_DELAY 16 // LIMIT_FPS = 1000 / LIMIT_DELAY
#define FFT_SIZE 1024
#define SAMPLE_RATE 44100

/*
   !!!IMPORTANT  : Enter your root folder path here
*/
std::string rootFolderPath = "../../";

int main(int argc, char* argv[])
{
    bool running = true;

    Timer timer;

    AudioManager audioManager(rootFolderPath + "database/audiodb.json");
    ResourceManager resourceManager;
    AudioCapture audio;

    Lengine::Window window("Audio Visualizer", 1280, 720, 0);
    Lengine::RenderPipeline renderPipeline(resourceManager);

    // Neither hosting nor receiving until the user picks a mode in the
    // Network panel (or you call StartHost/StartReceiver here to default).
    NetworkService network;

    Lengine::ImGuiLayer imguiLayer(
        running,
        window.getWindow(),
        window.getGlContext(),
        audioManager,
        audioManager.GetAudios(),
        audio,
        renderPipeline,
        network); 

    FFTProcessor fft(FFT_SIZE);
    AudioAnalyzer analyzer(FFT_SIZE);

    // Cache of the last frame received over the network, rendered
    // unconditionally each frame regardless of whether a new packet arrived
    // this tick (this is the render-loop decoupling fix already in place).
    VizPacket latestRemotePacket = MakeSilentPacket(0);

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

        // ---------------- Receiver path ----------------
        // Renders whatever the last-known remote packet is, independent of
        // whether a new one showed up this frame.
        if (network.IsReceiving())
        {
            if (auto frame = network.TryGetFrame())
            {
                latestRemotePacket = *frame;
            }

            renderPipeline.Render(
                latestRemotePacket.audioId,
                latestRemotePacket.bass,
                latestRemotePacket.mid,
                latestRemotePacket.treble,
                latestRemotePacket.spectrum,
                latestRemotePacket.smoothSpectrum,
                latestRemotePacket.logSpectrum, // no separate log-spectrum on the wire yet
                RenderMode::HOLOGRAM_WAVES,
                timer.GetTime());

            imguiLayer.renderViewport(
                latestRemotePacket.audioId,
                renderPipeline.GetFinalImage(latestRemotePacket.audioId),
                renderPipeline.GetRenderMode(latestRemotePacket.audioId));
        }

        // ---------------- Local audio path (also drives Host broadcast) ----------------
        for (auto& id : audioManager.GetActiveAudios())
        {
            auto sample_window = audio.GetSamplesWindow(id, FFT_SIZE);

            fft.Process(id, sample_window);

            const auto& spectrum = fft.GetSpectrum(id);

            analyzer.Analyze(id, spectrum, SAMPLE_RATE);

            float bass = analyzer.GetBass(id);
            float mid = analyzer.GetMid(id);
            float treble = analyzer.GetTreble(id);

            const auto& smoothedSpectrum = analyzer.GetSmoothedSpectrum(id);
            const auto& logSpectrum = analyzer.GetLogSpectrum(id);

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

            // ---- Broadcast this frame's analyzed data, if hosting ----
            if (network.IsHosting())
            {
                network.Broadcast(id, bass, mid, treble, smoothedSpectrum, smoothedSpectrum, logSpectrum);
            }
        }

        audioManager.Update(renderPipeline);

        imguiLayer.renderPanels();
        imguiLayer.endFrame();

        SDL_Delay(LIMIT_DELAY);
        window.swapBuffer();
    }

    window.quitWindow();
    SDL_Quit();

    network.Shutdown();

    return 0;
}