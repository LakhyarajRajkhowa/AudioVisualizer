#pragma once
#include <SDL/SDL.h>
#define IMGUI_ENABLE_DOCKING

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "panels/ViewportPanel.h"
#include "panels/AudioLibraryPanel.h"

#include "utils/Paths.h"


namespace Lengine {
	class ImGuiLayer {
	public:
		ImGuiLayer(
			bool& run,
			SDL_Window* window,
			SDL_GLContext glContext,
			AudioManager& am,
			const std::unordered_map<int, AudioMeta>& db,
			std::unordered_set<int>& aa,
			AudioCapture& ac,
			Lengine::RenderPipeline& rp
			) :
			isRunning(run),
			audioLibraryPanel(db, aa, ac, am, playPanel),
			audioDatabase(db)
		{
			init(window, glContext);
		};
		void init(SDL_Window* window, SDL_GLContext glContext);
		void shutdown();
		// process event and return true if ImGui consumed it (optional)
		void processEvent(const SDL_Event& e);
		void beginFrame();
		void renderPanels();
		void renderViewport(const int id, const uint32_t finalImage);

		void endFrame();


		// helper
		bool wantsCaptureMouse() const;
		bool wantsCaptureKeyboard() const;

		// Theme
		void SetModernDarkTheme();
	private:
		bool& isRunning;
		
		ViewportPanel  viewport;
		AudioLibraryPanel audioLibraryPanel;
		PlayPanel playPanel;

		const std::unordered_map<int, AudioMeta>& audioDatabase;

		void BeginDockspace();


	};
}
