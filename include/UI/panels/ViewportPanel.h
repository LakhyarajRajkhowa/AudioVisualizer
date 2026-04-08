#pragma once

#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>

#define IMGUI_ENABLE_DOCKING
#define IMGUI_ENABLE_DOCKING_EXTENSION
#include "imgui.h"
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace Lengine
{
	class ViewportPanel {
	public:
		void OnImGuiRender(const int id, const std::string name, const uint32_t finalImage);
	private:
		ImVec2 viewportSize = { 1920, 1080 };

	};
}