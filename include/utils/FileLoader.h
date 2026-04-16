#pragma once

#include <tinyfiledialogs.h>
#include <string>

inline std::string OpenAudioFileDialog()
{
    const char* filters[] = { "*.wav", "*.mp3", "*.ogg" };

    const char* path = tinyfd_openFileDialog(
        "Import Audio",
        "",
        3,
        filters,
        "Audio Files",
        0
    );

    if (path)
        return std::string(path);

    return "";
}