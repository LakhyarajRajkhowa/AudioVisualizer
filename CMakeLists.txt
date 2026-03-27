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

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
    external/SDL
    external/GL
)

target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLEW
    IMGUI_IMPL_SDL2_SDL2
)

# -------------------------------------------------
# KISSFFT
# -------------------------------------------------

set(KISSFFT_SRC
    external/kissfft/kiss_fft.c
    external/kissfft/kiss_fftr.c
)

# -------------------------------------------------
# Main Source
# -------------------------------------------------

set(SRC_FILES
    main.cpp
    src/audio/AudioCapture.cpp
    src/audio/FFTProcessor.cpp
    src/audio/AudioAnalyzer.cpp
    src/visualizer/opengl/GLSLProgram.cpp
    src/UI/Window.cpp
    src/UI/ImguiLayer.cpp

)

# -------------------------------------------------
# Executable
# -------------------------------------------------

add_executable(AudioVisualizer
    ${SRC_FILES}
    ${KISSFFT_SRC}
)

# -------------------------------------------------
# Include directories
# -------------------------------------------------

target_include_directories(AudioVisualizer PRIVATE
    include
    external

)

# -------------------------------------------------
# Link external libraries
# -------------------------------------------------

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(AudioVisualizer
    imgui
    SDL2
    glew32
    opengl32
)