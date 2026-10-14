#include "audio/AudioManager.h"

#include <system_error>

using json = nlohmann::json;

void AudioManager::Update(Lengine::RenderPipeline& rp)
{
    while (!audioToBeUnactivated.empty())
    {
        const int id = audioToBeUnactivated.front();
        activeAudios.erase(id);
        isPlaying[id] = false;
        rp.Destroy(id);
        audioToBeUnactivated.pop();
    }
}

std::string AudioManager::ExtractName(const std::string& path)
{
    return std::filesystem::path(path).stem().string();
}

int AudioManager::ImportAudio(const std::string& path)
{
    if (path.empty())
    {
        std::cerr << "[Audio Error] Cannot import an empty file path.\n";
        return -1;
    }

    std::error_code ec;
    const std::filesystem::path filePath(path);
    if (!std::filesystem::exists(filePath, ec) || ec ||
        !std::filesystem::is_regular_file(filePath, ec) || ec)
    {
        std::cerr << "[Audio Error] Cannot import audio; path is missing or is not a file: "
                  << path << '\n';
        return -1;
    }

    AudioMeta meta;
    meta.id = nextID++;
    meta.filepath = path;
    meta.name = ExtractName(path);

    audioDB[meta.id] = meta;
    SaveDatabase();

    std::cout << "Imported Audio: " << meta.name << " ID: " << meta.id << '\n';
    return meta.id;
}

void AudioManager::SaveDatabase()
{
    json j;
    j["audios"] = json::array();

    for (const auto& [id, audio] : audioDB)
    {
        j["audios"].push_back({
            {"id", audio.id},
            {"name", audio.name},
            {"filepath", audio.filepath}
        });
    }

    std::ofstream file(dbPath);
    if (!file.is_open())
    {
        std::cerr << "[Audio Database Error] Could not write database: "
                  << dbPath << '\n';
        return;
    }

    file << j.dump(4);
    if (!file)
        std::cerr << "[Audio Database Error] Failed while writing database: "
                  << dbPath << '\n';
}

void AudioManager::LoadDatabase()
{
    std::ifstream file(dbPath);
    if (!file.is_open())
    {
        std::cerr << "[Audio Database] No database found at '" << dbPath
                  << "'. Starting with an empty library.\n";
        return;
    }

    try
    {
        json j;
        file >> j;

        if (!j.is_object() || !j.contains("audios") || !j["audios"].is_array())
        {
            std::cerr << "[Audio Database Error] Invalid database structure in '"
                      << dbPath << "'. Starting with an empty library.\n";
            return;
        }

        for (const auto& item : j["audios"])
        {
            if (!item.is_object() ||
                !item.contains("id") || !item["id"].is_number_integer() ||
                !item.contains("name") || !item["name"].is_string() ||
                !item.contains("filepath") || !item["filepath"].is_string())
            {
                std::cerr << "[Audio Database Warning] Skipping malformed audio entry.\n";
                continue;
            }

            AudioMeta meta;
            meta.id = item["id"].get<int>();
            meta.name = item["name"].get<std::string>();
            meta.filepath = item["filepath"].get<std::string>();

            if (meta.id < 0 || meta.filepath.empty())
            {
                std::cerr << "[Audio Database Warning] Skipping entry with invalid ID/path.\n";
                continue;
            }

            std::error_code ec;
            if (!std::filesystem::is_regular_file(meta.filepath, ec) || ec)
            {
                std::cerr << "[Audio Database Warning] Asset is currently unavailable: "
                          << meta.filepath << '\n';
                // Keep the entry visible so the UI can report a clear load error.
            }

            audioDB[meta.id] = meta;
            if (meta.id >= nextID)
                nextID = meta.id + 1;
        }

        std::cout << "Loaded " << audioDB.size() << " audio entries\n";
    }
    catch (const json::exception& e)
    {
        std::cerr << "[Audio Database Error] Could not parse '" << dbPath
                  << "': " << e.what() << ". Starting with an empty library.\n";
    }
    catch (const std::ios_base::failure& e)
    {
        std::cerr << "[Audio Database Error] Failed reading '" << dbPath
                  << "': " << e.what() << '\n';
    }
}
