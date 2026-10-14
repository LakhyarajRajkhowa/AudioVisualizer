#include "UI/panels/AudioLibraryPanel.h"

#include <iostream>

void AudioLibraryPanel::Draw()
{
    for (auto it = audioManager.activeAudios.begin(); it != audioManager.activeAudios.end(); )
    {
        const int id = *it;
        auto meta = audioManager.audioDB.find(id);
        if (meta == audioManager.audioDB.end())
        {
            it = audioManager.activeAudios.erase(it);
            continue;
        }

        playPanel.Draw(
            audioCapture,
            audioManager.audioToBeUnactivated,
            id,
            meta->second.name
        );
        ++it;
    }

    ImGui::Begin("Audio Library");

    if (ImGui::BeginPopupContextWindow("AudioLibraryContext", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Import Audio"))
        {
            const std::string path = OpenAudioFileDialog();
            if (!path.empty())
                audioManager.ImportAudio(path);
        }
        ImGui::EndPopup();
    }

    if (audioManager.audioDB.empty())
        ImGui::Text("No Audio Loaded");

    const float cellSize = iconSize + padding;
    const float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, nullptr, false);

    for (const auto& [id, audio] : audioManager.audioDB)
    {
        ImGui::PushID(id);
        ImGui::BeginGroup();

        const bool audioClicked = iconTexture != 0
            ? ImGui::ImageButton(
                "##audio_icon",
                (ImTextureID)(intptr_t)iconTexture,
                ImVec2(iconSize, iconSize))
            : ImGui::Button("Audio", ImVec2(iconSize, iconSize));

        if (audioClicked)
        {
            if (!audioManager.activeAudios.count(audio.id))
            {
                // Only activate an entry after the decoder confirms that it
                // can open and decode the file.
                if (!audioCapture.LoadAudio(audio.id, audio.filepath))
                {
                    ImGui::OpenPopup("AudioLoadError");
                }
                else
                {
                    audioManager.activeAudios.insert(audio.id);
                    audioCapture.Play(audio.id);
                    audioManager.isPlaying[audio.id] = audioCapture.IsPlaying(audio.id);
                }
            }
            else
            {
                audioCapture.Play(audio.id);
                audioManager.isPlaying[audio.id] = audioCapture.IsPlaying(audio.id);
            }
        }

        if (ImGui::BeginPopup("AudioLoadError"))
        {
            ImGui::TextWrapped("Could not load this audio file. Check that the file still exists and uses a supported audio format.");
            ImGui::TextWrapped("%s", audio.filepath.c_str());
            if (ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        const float textWidth = ImGui::CalcTextSize(audio.name.c_str()).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (iconSize - textWidth) * 0.5f);
        ImGui::TextWrapped("%s", audio.name.c_str());

        ImGui::EndGroup();
        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
    ImGui::End();
}