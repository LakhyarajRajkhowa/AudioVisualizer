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
    external/SDL
    external/GL
)

source_group("external\\imgui" FILES ${IMGUI_SRC})

# -------------------------------------------------
# KISSFFT
# -------------------------------------------------

set(KISSFFT_SRC
    external/kissfft/kiss_fft.c
    external/kissfft/kiss_fftr.c
)

source_group("external\\kissfft" FILES ${KISSFFT_SRC})

# -------------------------------------------------
# Source Files
# -------------------------------------------------

set(AUDIO_SRC
    src/audio/AudioCapture.cpp
    src/audio/FFTProcessor.cpp
    src/audio/AudioAnalyzer.cpp
    include/audio/AudioCapture.h
    include/audio/FFTProcessor.h
    include/audio/AudioAnalyzer.h
)

set(UI_SRC
    src/UI/Window.cpp
    src/UI/ImguiLayer.cpp
    include/UI/Window.h
    include/UI/ImguiLayer.h
)

set(VISUALIZER_SRC
    src/visualizer/opengl/GLSLProgram.cpp
    include/visualizer/opengl/GLSLProgram.h

)

set(ROOT_SRC
    main.cpp
)

set(SRC_FILES
    ${ROOT_SRC}
    ${AUDIO_SRC}
    ${UI_SRC}
    ${VISUALIZER_SRC}
)

# -------------------------------------------------
# Visual Studio Filters
# -------------------------------------------------

source_group("src\\audio" FILES ${AUDIO_SRC})
source_group("src\\UI" FILES ${UI_SRC})
source_group("src\\visualizer\\opengl" FILES ${VISUALIZER_SRC})
source_group("root" FILES ${ROOT_SRC})

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

# -------------------------------------------------
# Link external libraries
# -------------------------------------------------

target_link_directories(AudioVisualizer PRIVATE lib)

target_link_libraries(AudioVisualizer
    imgui
    SDL2
    glew32
    opengl32
)
# -------------------------------------------------
# Output directory
# -------------------------------------------------

set_target_properties(AudioVisualizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)