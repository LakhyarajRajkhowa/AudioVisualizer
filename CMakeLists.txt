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
)

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
    include/UI/Window.h
    include/UI/ImguiLayer.h
    include/UI/panels/ViewportPanel.h
    include/UI/panels/AudioLibraryPanel.h
    include/UI/panels/AudioPlayPanel.h

)

set(VISUALIZER_SRC
    src/visualizer/opengl/GLSLProgram.cpp
    include/visualizer/opengl/GLSLProgram.h

    src/visualizer/framebuffers/Framebuffer.cpp
    include/visualizer/framebuffers/Framebuffer.h

    src/visualizer/RenderPipeline.cpp
    include/visualizer/RenderPipeline.h

)

set(ROOT_SRC
    main.cpp
)

set(SRC_FILES
    ${ROOT_SRC}
    ${AUDIO_SRC}
    ${UI_SRC}
    ${VISUALIZER_SRC}
    ${UTILS_SRC}
)

# -------------------------------------------------
# Executable
# -------------------------------------------------

add_executable(AudioVisualizer
    ${SRC_FILES}
    ${KISSFFT_SRC}
    ${TINYFILE_SRC}
)

# -------------------------------------------------
# Include directories
# -------------------------------------------------

target_include_directories(AudioVisualizer PRIVATE
    include
    external
)

# -------------------------------------------------
# Link libraries
# -------------------------------------------------

target_link_directories(AudioVisualizer PRIVATE lib)

target_link_libraries(AudioVisualizer
    imgui
    SDL2
    glew32
    opengl32
)

# -------------------------------------------------
# Mirror folder structure in Visual Studio
# -------------------------------------------------

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${SRC_FILES}
    ${KISSFFT_SRC}
    ${IMGUI_SRC}
    ${TINYFILE_SRC}
)

# -------------------------------------------------
# Output directory
# -------------------------------------------------

set_target_properties(AudioVisualizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)