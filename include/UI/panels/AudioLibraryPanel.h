#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <string>
#include <imgui.h>

#include "audio/AudioCapture.h"
#include "audio/AudioManager.h"

#include "utils/TextureLoader.h"
#include "utils/Paths.h"
#include "utils/FileLoader.h"

#include "AudioPlayPanel.h"

#include "visualizer/RenderPipeline.h"

class AudioLibraryPanel
{
public:
    AudioLibraryPanel(
        AudioCapture& ac ,
        AudioManager& am,
        PlayPanel& pp
     
        ) :
        audioCapture(ac),
        audioManager(am),
        playPanel(pp)
        
    {
        iconTexture = LoadTexture(rootFolderPath + "assets/icons/audio_icon.png");
    }


    void Draw();

private:

    AudioCapture& audioCapture;
    AudioManager& audioManager;
    PlayPanel& playPanel;
    
    unsigned int iconTexture = 0;

    float iconSize = 64.0f;
    float padding = 16.0f;


};