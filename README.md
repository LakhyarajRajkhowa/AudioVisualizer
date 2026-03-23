# Audio Visualizer

A simple OpenGL audio visualizer built with **SDL2**, **GLEW**, and **C++**.
The program loads an MP3 file, performs **FFT analysis**, and prepares the data for visualization.

---

# Dependencies

## Windows (MSYS2)

1. Install **MSYS2** from:
   https://www.msys2.org/

2. Open **MSYS2 MinGW64** terminal and install dependencies:

```
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-ninja
```

---

# Build Instructions

```
git clone https://github.com/LakhyarajRajkhowa/AudioVisualizer.git

mkdir build
cd build

cmake -G Ninja ..
cmake --build .
```

---


# Run

After building:

```
./AudioVisualizer.exe
```

---

# Project Structure

```
AudioVisualizer/
│
├── assets/        # Audio files
├── external/      # Third-party libraries (kissfft, imgui, etc.)
├── include/       # Headers
├── src/           # Source files
├── main.cpp
├── CMakeLists.txt
└── README.md
```
