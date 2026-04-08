#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>

struct AudioMeta
{
    int id;
    std::string name;
    std::string filepath;
};

class AudioManager
{
public:
    AudioManager(std::string dbPath) : dbPath(dbPath){}
    void LoadDatabase();
    void SaveDatabase();

    int ImportAudio(const std::string& path);

    const std::unordered_map<int, AudioMeta>& GetAudios() const
    {
        return audioDB;
    }

    std::unordered_set<int>& GetActiveAudios()
    {
        return activeAudios;
    }

    const std::unordered_set<int>& GetActiveAudios() const
    {
        return activeAudios;
    }

private:

    std::unordered_map<int, AudioMeta> audioDB;

    std::unordered_set<int> activeAudios;

    int nextID = 1;

    std::string ExtractName(const std::string& path);

    const std::string dbPath;
};