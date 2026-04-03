# Audio Visualizer

A simple OpenGL audio visualizer built with **SDL2**, **GLEW**, and **C++**.
The program loads an MP3 file, performs **FFT analysis**, and prepares the data for visualization.

---

# Build Instructions

```
git clone https://github.com/LakhyarajRajkhowa/AudioVisualizer.git
cd AudioVisaulizer

mkdir build
cd build

cmake ..
cmake --build .
```

---


# Run

After building:

> Copy AudioVisualizer/build/bin/Debug/AudioVisualizer.exe to 
     AudioVisualizer/bin

> Click on AudioVisualizer.exe

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
