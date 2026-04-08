#include "UI/panels/AudioPlayPanel.h"
#include <imgui.h>

void PlayPanel::Draw(AudioCapture& audio, std::queue<int>& audioToBeUnactivated, const int id, const std::string name)
{
    ImGui::Begin("Play");
    ImGui::PushID(id);

    ImGui::Separator();
    ImGui::Text(name.c_str());

    // --- Controls ---

    if (!isPlaying[id])
    {
        if (ImGui::Button("Play"))
        {
            audio.Play(id);
            isPlaying[id] = true;
        }
    }
    else
    {
        if (ImGui::Button("Pause" ))
        {
            audio.Pause(id);
            isPlaying[id] = false;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        audioToBeUnactivated.push(id);
        audio.Stop(id);
    }

    ImGui::Separator();

    // --- Timeline ---
    uint64_t currentFrame = audio.GetPlaybackFrame(id);
    uint64_t totalFrames = audio.GetTotalFrames(id);

    const float sampleRate = audio.GetSampleRate(id);

    float currentSec = currentFrame / sampleRate;
    float totalSec = totalFrames / sampleRate;

    float time = currentSec;

    if (ImGui::SliderFloat("Timeline", &time, 0.0f, totalSec, ""))
    {
        uint64_t frame = (uint64_t)(time * sampleRate);
        audio.SeekFrame(id, frame);
    }

    int currentMin = (int)currentSec / 60;
    int currentSecOnly = (int)currentSec % 60;

    int totalMin = (int)totalSec / 60;
    int totalSecOnly = (int)totalSec % 60;

    ImGui::Text("Time: %02d:%02d / %02d:%02d",
        currentMin, currentSecOnly,
        totalMin, totalSecOnly);

    ImGui::Separator();

    ImGui::PopID();

    ImGui::End();
}