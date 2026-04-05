#include "UI/ImGuiLayer.h"


using namespace Lengine;
void ImGuiLayer::init(SDL_Window* window, SDL_GLContext glContext) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 

    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    SetModernDarkTheme();
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");

    
}


void ImGuiLayer::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}



void ImGuiLayer::beginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

}

void ImGuiLayer::renderPanels(const uint32_t finalImage) {
    BeginDockspace();

    viewport.OnImGuiRender(finalImage);
}


void ImGuiLayer::endFrame() {
   
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

}


bool ImGuiLayer::wantsCaptureMouse() const {
	return ImGui::GetIO().WantCaptureMouse;
}


bool ImGuiLayer::wantsCaptureKeyboard() const {
	return ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiLayer::SetModernDarkTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base background
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Headers (Tree, CollapsingHeader, etc.)
    colors[ImGuiCol_Header] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

    // Frame backgrounds (input boxes, sliders, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    // Title bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.08f, 0.80f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.20f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);

    // Menubar
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);

    // Check marks / sliders
    colors[ImGuiCol_CheckMark] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);

    // Modern flat feel
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 2.0f;
    style.ScrollbarRounding = 6.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;

    style.TabBorderSize = 0.0f;
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.Fonts->AddFontFromFileTTF("../assets/fonts/roboto-font/RobotoRegular-3m4L.ttf", 15.0f);

}

// Dockspace
void ImGuiLayer::BeginDockspace()
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_MenuBar;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("MainDockspace", nullptr, window_flags);

    ImGui::PopStyleVar(2);


    ImGuiID dockspace_id = ImGui::GetID("MainDockspaceID");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0));

    ImGui::End();
}