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

#include "AudioPlayPanel.h"

#include "visualizer/RenderPipeline.h"

class AudioLibraryPanel
{
public:
    AudioLibraryPanel(
        const std::unordered_map<int, AudioMeta>& db,
        std::unordered_set<int>& aa,
        AudioCapture& ac ,
        PlayPanel& pp,
        Lengine::RenderPipeline& rp
     
        ) :
        audioDB(db),
        activeAudios(aa),
        audioCapture(ac),
        playPanel(pp),
        renderPipeline(rp)
        
    {
        iconTexture = LoadTexture(rootFolderPath + "assets/icons/audio_icon.png");
    }


    void Draw();

private:

    const std::unordered_map<int, AudioMeta>& audioDB;
    std::unordered_set<int>& activeAudios;

    std::queue<int> audioToBeUnactivated;

    AudioCapture& audioCapture;
    PlayPanel& playPanel;
    
    Lengine::RenderPipeline& renderPipeline;
    unsigned int iconTexture = 0;

    float iconSize = 64.0f;
    float padding = 16.0f;


};