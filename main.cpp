#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>

#include "include/audio/AudioCapture.h"
#include "include/audio/FFTProcessor.h"
#include "include/UI/Window.h"

using namespace std;

int main(int argc, char* argv[])
{
    // -------- Audio --------
    AudioCapture audio;
    audio.LoadFile("../assets/audio/shape_of_you.mp3");
    audio.Play();

    // -------- Window --------
    Lengine::Window window("Audio Visualizer Test", 1280, 720, 0);

    // -------- FFT --------
    FFTProcessor fft(1024);

    bool running = true;
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

      
        // -------- Rendering --------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap buffers
        window.swapBuffer();
    }

    window.quitWindow();
    SDL_Quit();

    return 0;
}