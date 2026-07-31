cmake_minimum_required(VERSION 3.16)

project(AudioVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------
# ImGui Library
# -------------------------------------------------

set(IMGUI_SRC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

set(TINYFILE_SRC
    external/tinyfiledialogs/tinyfiledialogs.c
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    external/imgui
    external/SDL
    external/GL
    external/nlohmann
    external/tinyfiledialogs
    external/stb
    external/tinyobjloader
)

# -------------------------------------------------
# NetCore
# -------------------------------------------------

add_subdirectory(external/LNet)


# -------------------------------------------------
# KISSFFT
# -------------------------------------------------

set(KISSFFT_SRC
    external/kissfft/kiss_fft.c
    external/kissfft/kiss_fftr.c
)

# -------------------------------------------------
# Source Files
# -------------------------------------------------

set(UTILS_SRC
    include/utils/TextureLoader.h
    include/utils/Paths.h
    include/utils/ObjLoader.h
    include/utils/Timer.h
    include/utils/FileLoader.h
)

set(RESOURCES_SRC
    include/resources/ResourceManager.h
    include/resources/Mesh.h
    src/resources/Mesh.cpp
)

set(AUDIO_SRC
    src/audio/AudioCapture.cpp
    src/audio/FFTProcessor.cpp
    src/audio/AudioAnalyzer.cpp
    src/audio/AudioManager.cpp
    include/audio/AudioCapture.h
    include/audio/FFTProcessor.h
    include/audio/AudioAnalyzer.h
    include/audio/AudioManager.h
)

set(UI_SRC
    src/UI/Window.cpp
    src/UI/ImguiLayer.cpp
    src/UI/panels/ViewportPanel.cpp
    src/UI/panels/AudioLibraryPanel.cpp
    src/UI/panels/AudioPlayPanel.cpp
    src/UI/panels/NetworkPanel.cpp
    include/UI/Window.h
    include/UI/ImguiLayer.h
    include/UI/panels/ViewportPanel.h
    include/UI/panels/AudioLibraryPanel.h
    include/UI/panels/AudioPlayPanel.h
    include/UI/panels/NetworkPanel.h
)

set(VISUALIZER_SRC
    src/visualizer/opengl/GLSLProgram.cpp
    include/visualizer/opengl/GLSLProgram.h

    src/visualizer/framebuffers/Framebuffer.cpp
    include/visualizer/framebuffers/Framebuffer.h

    src/visualizer/RenderPipeline.cpp
    include/visualizer/RenderPipeline.h

    src/visualizer/Renderer.cpp
    include/visualizer/Renderer.h

    src/visualizer/WaveBarRenderer.cpp
    include/visualizer/WaveBarRenderer.h

    src/visualizer/RingRenderer.cpp
    include/visualizer/RingRenderer.h

    src/visualizer/SphereMeshRenderer.cpp
    include/visualizer/SphereMeshRenderer.h
)

set(NETWORK_SRC
    include/network/NetworkService.h
    src/network/NetworkService.cpp
)

# -------------------------------------------------
# Shared Library
# -------------------------------------------------

add_library(VisualizerCore STATIC
    ${AUDIO_SRC}
    ${UI_SRC}
    ${VISUALIZER_SRC}
    ${UTILS_SRC}
    ${NETWORK_SRC}
    ${RESOURCES_SRC}
    ${KISSFFT_SRC}
    ${TINYFILE_SRC}
)

target_include_directories(VisualizerCore PUBLIC
    include
    external
)

target_link_libraries(VisualizerCore
    PUBLIC
        imgui
        SDL2
        GLEW
        GL
        LNet
)


# -------------------------------------------------
#  Executable
# -------------------------------------------------

add_executable(AudioVisualizer
    main.cpp
)

target_link_libraries(AudioVisualizer
    PRIVATE
        VisualizerCore
)

# -------------------------------------------------
# Visual Studio Folder Organization
# -------------------------------------------------

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${AUDIO_SRC}
    ${UI_SRC}
    ${VISUALIZER_SRC}
    ${UTILS_SRC}
    ${RESOURCES_SRC}
    ${KISSFFT_SRC}
    ${IMGUI_SRC}
    ${TINYFILE_SRC}
)

# -------------------------------------------------
# Output Directory
# -------------------------------------------------

set_target_properties(
    AudioVisualizer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)