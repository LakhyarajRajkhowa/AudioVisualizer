#include "UI/panels/AudioLibraryPanel.h"

void AudioLibraryPanel::Draw()
{
    while (!audioToBeUnactivated.empty()) {
        activeAudios.erase(audioToBeUnactivated.front());
        playPanel.isPlaying[audioToBeUnactivated.front()] = false;

        audioToBeUnactivated.pop();

    }

    for (auto& id : activeAudios) {
        playPanel.Draw(audioCapture, audioToBeUnactivated, id, audioDB.at(id).name);
    }

    ImGui::Begin("Audio Library");

    if (ImGui::BeginPopupContextWindow("AudioLibraryContext", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Import Audio"))
        {
            std::string path = OpenAudioFileDialog();

            if (!path.empty())
            {
                audioManager.ImportAudio(path);
            }
        }

        ImGui::EndPopup();
    }

    if (audioDB.empty()) {
        ImGui::Text("No Audio Loaded");
    }

    float cellSize = iconSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;

    int columnCount = (int)(panelWidth / cellSize);

    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto& [id, audio] : audioDB)
    {
        ImGui::PushID(id);

        ImGui::BeginGroup();

        // clickable icon
        if (ImGui::ImageButton(
            "##audio_icon",
            (ImTextureID)(intptr_t)iconTexture,
            ImVec2(iconSize, iconSize)))
        {
            if (!activeAudios.count(audio.id)) {
                audioCapture.LoadAudio(audio.id, audio.filepath);
                activeAudios.insert(audio.id);
            }

            audioCapture.Play(audio.id);
            playPanel.isPlaying[audio.id] = true;

        }

        // center text under icon
        float textWidth = ImGui::CalcTextSize(audio.name.c_str()).x;
        float avail = iconSize;

        ImGui::SetCursorPosX(
            ImGui::GetCursorPosX() + (avail - textWidth) * 0.5f);

        ImGui::TextWrapped("%s", audio.name.c_str());

        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::End();
}