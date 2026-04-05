
#include "audio/AudioManager.h"

using json = nlohmann::json;

std::string AudioManager::ExtractName(const std::string& path)
{
    std::filesystem::path p(path);
    return p.stem().string();
}

int AudioManager::ImportAudio(const std::string& path)
{
    AudioMeta meta;

    meta.id = nextID++;
    meta.filepath = path;
    meta.name = ExtractName(path);

    audioDB[meta.id] = meta;

    SaveDatabase();

    std::cout << "Imported Audio: " << meta.name << " ID: " << meta.id << "\n";

    return meta.id;
}

void AudioManager::SaveDatabase()
{
    json j;

    for (auto& [id, audio] : audioDB)
    {
        j["audios"].push_back({
            {"id", audio.id},
            {"name", audio.name},
            {"filepath", audio.filepath}
            });
    }

    std::ofstream file(dbPath);
    file << j.dump(4);
}

void AudioManager::LoadDatabase()
{
    std::ifstream file(dbPath);

    if (!file.is_open())
    {
        std::cout << "No audio database found\n";
        return;
    }

    json j;
    file >> j;

    for (auto& item : j["audios"])
    {
        AudioMeta meta;

        meta.id = item["id"];
        meta.name = item["name"];
        meta.filepath = item["filepath"];

        audioDB[meta.id] = meta;

        if (meta.id >= nextID)
            nextID = meta.id + 1;
    }

    std::cout << "Loaded " << audioDB.size() << " audio files\n";
}