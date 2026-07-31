cmake_minimum_required(VERSION 3.16)
project(LNet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sender<T>/Receiver<T>/JitterBuffer<T>/etc. are header-only templates, so the
# only translation units the library actually compiles are the non-template
# pieces: Socket and Logger.
add_library(LNet STATIC
    src/lnet/Socket.cpp
    src/logging/Logger.cpp
)

target_include_directories(LNet PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(LNet PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(LNet PUBLIC ws2_32)
endif()
