#pragma once

#include <queue>
#include "audio/AudioCapture.h"

class PlayPanel
{
public:

    void Draw(AudioCapture& audio, std::queue<int>& audioToBeUnactivated, const int id, const std::string name);

    std::unordered_map<int, bool>  isPlaying;


private:
   

};