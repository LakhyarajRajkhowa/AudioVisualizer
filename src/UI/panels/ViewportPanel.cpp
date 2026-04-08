#include "UI/panels/ViewportPanel.h"

using namespace Lengine; 

void ViewportPanel::OnImGuiRender(const int id, const std::string name, const uint32_t finalImage)
{
    std::string windowName = name + "##Viewport" + std::to_string(id);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (ImGui::Begin(windowName.c_str()))
    {
        ImGui::Separator();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        viewportSize = { avail.x, avail.y };

        GLuint texID = finalImage;

        ImGui::Image(
            (void*)(intptr_t)texID,
            ImVec2(viewportSize.x, viewportSize.y),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    }

    ImGui::End();
    ImGui::PopStyleVar();
}