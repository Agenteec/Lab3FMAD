#include "SaveSystem.h"
#include "raylib.h"
#include <iostream>
#include <fstream>
#include <ctime>

#ifdef _WIN32
#include <direct.h>
#define CREATE_DIRECTORY(path) _mkdir(path.c_str())
#else
#include <sys/stat.h>
#define CREATE_DIRECTORY(path) mkdir(path.c_str(), 0755)
#endif

SaveSystem::SaveSystem()
        : saveDirectory("saves"),
          currentProfile("default") {
    Initialize();
}

void SaveSystem::Initialize() {
    // Создаем директорию для сохранений, если её нет
    if (!DirectoryExists(saveDirectory)) {
        CreateDirectory(saveDirectory);
    }

    // Загружаем список профилей
    // В реальной реализации нужно сканировать директорию
    playerProfiles["default"] = PlayerStats{"Player", 0, 0, 0, 0.0f, ""};
}

bool SaveSystem::SaveGame(const Game& game, const std::string& profileName) {
    std::string filePath = GetSaveFilePath(profileName);
    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << filePath << std::endl;
        return false;
    }

    try {
        // Сохраняем игровые данные
        int wave = game.GetCurrentWave();
        int resources = game.GetResources();
        int score = game.GetPlayerScore();
        float playTime = game.GetGameTime();

        file.write(reinterpret_cast<const char*>(&wave), sizeof(wave));
        file.write(reinterpret_cast<const char*>(&resources), sizeof(resources));
        file.write(reinterpret_cast<const char*>(&score), sizeof(score));
        file.write(reinterpret_cast<const char*>(&playTime), sizeof(playTime));

        // Сохраняем статистику игрока
        PlayerStats& stats = playerProfiles[profileName];
        stats.highestWave = std::max(stats.highestWave, wave);
        stats.totalScore += score;
        stats.totalPlayTime += playTime;

        // Дата сохранения
        std::time_t now = std::time(nullptr);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        stats.lastSaveDate = timeStr;

        file.close();
        std::cout << "Game saved successfully: " << filePath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving game: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool SaveSystem::LoadGame(Game& game, const std::string& profileName) {
    std::string filePath = GetSaveFilePath(profileName);
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << filePath << std::endl;
        return false;
    }

    try {
        int wave, resources, score;
        float playTime;

        file.read(reinterpret_cast<char*>(&wave), sizeof(wave));
        file.read(reinterpret_cast<char*>(&resources), sizeof(resources));
        file.read(reinterpret_cast<char*>(&score), sizeof(score));
        file.read(reinterpret_cast<char*>(&playTime), sizeof(playTime));

        // Восстанавливаем состояние игры
        game.SetCurrentWave(wave);
        // game.SetResources(resources); // Нужны сеттеры в Game
        // game.SetPlayerScore(score);
        // game.SetGameTime(playTime);

        file.close();
        std::cout << "Game loaded successfully: " << filePath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool SaveSystem::SaveSettings(const GameSettings& settings) {
    std::string filePath = GetSettingsFilePath();
    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    try {
        file.write(reinterpret_cast<const char*>(&settings), sizeof(settings));
        file.close();
        return true;
    } catch (...) {
        file.close();
        return false;
    }
}

bool SaveSystem::LoadSettings(GameSettings& settings) {
    std::string filePath = GetSettingsFilePath();
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    try {
        file.read(reinterpret_cast<char*>(&settings), sizeof(settings));
        file.close();
        return true;
    } catch (...) {
        file.close();
        return false;
    }
}

void SaveSystem::CreateProfile(const std::string& profileName) {
    playerProfiles[profileName] = PlayerStats{profileName, 0, 0, 0, 0.0f, ""};
}

void SaveSystem::DeleteProfile(const std::string& profileName) {
    playerProfiles.erase(profileName);

    // Удаляем файл сохранения
    std::string filePath = GetSaveFilePath(profileName);
    remove(filePath.c_str());
}

std::vector<std::string> SaveSystem::GetAvailableProfiles() const {
    std::vector<std::string> profiles;
    for (const auto& pair : playerProfiles) {
        profiles.push_back(pair.first);
    }
    return profiles;
}

SaveSystem::PlayerStats SaveSystem::GetPlayerStats(const std::string& profileName) const {
    auto it = playerProfiles.find(profileName);
    if (it != playerProfiles.end()) {
        return it->second;
    }
    return PlayerStats{};
}

std::string SaveSystem::GetSaveFilePath(const std::string& profileName) const {
    return saveDirectory + "/" + profileName + ".sav";
}

std::string SaveSystem::GetSettingsFilePath() const {
    return saveDirectory + "/settings.cfg";
}

bool SaveSystem::DirectoryExists(const std::string& path) const {
    // Упрощенная проверка - в реальном проекте используйте std::filesystem
    std::ifstream test(path + "/test");
    return test.good();
}

bool SaveSystem::CreateDirectory(const std::string& path) const {
    return CREATE_DIRECTORY(path) == 0;
}