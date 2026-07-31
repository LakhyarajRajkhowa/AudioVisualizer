#pragma once

#include <queue>
#include <unordered_map>

#include "audio/AudioCapture.h"


class PlayPanel
{
public:

    void Draw(
        AudioCapture& audio,
        std::queue<int>& audioToBeUnactivated,
        const int id, const std::string name
    );


private:
   

};