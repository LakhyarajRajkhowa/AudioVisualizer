#pragma once

#include <unordered_map>
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

private:

    std::unordered_map<int, AudioMeta> audioDB;

    int nextID = 1;

    std::string ExtractName(const std::string& path);

    const std::string dbPath;
};