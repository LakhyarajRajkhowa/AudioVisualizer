#pragma once
#include <cstdint>
#include "../external/SDL/SDL.h"
#include "../external/GL/glew.h"
#include <string>

#include "../external/imgui/backends/imgui_impl_sdl2.h"
#include "../external/imgui/backends/imgui_impl_opengl3.h"


namespace Lengine {

	enum  windowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

	class Window
	{
	public:
		Window(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);
		~Window();

		int Create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);

		void swapBuffer();
		void quitWindow();

		uint32_t getScreenWidth() { return screenWidth; }
		uint32_t getScreenHeight() { return screenHeight; }

		SDL_Window* getWindow() { return _sdlWindow; }
		SDL_GLContext getGlContext() { return _glContext;  }

	private:
		
		SDL_Window* _sdlWindow;
		SDL_GLContext _glContext = nullptr;
		uint32_t screenWidth, screenHeight;

	};
}



