#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <string>
#include <unordered_map>
#include "Game.h"
class GameSettings;
class SaveSystem {
private:
    std::string saveDirectory;
    std::string currentProfile;

    struct PlayerStats {
        std::string playerName;
        int highestWave;
        int totalScore;
        int totalKills;
        float totalPlayTime;
        std::string lastSaveDate;
    };

    std::unordered_map<std::string, PlayerStats> playerProfiles;

public:
    SaveSystem();

    void Initialize();
    bool SaveGame(const Game& game, const std::string& profileName = "default");
    bool LoadGame(Game& game, const std::string& profileName = "default");
    bool SaveSettings(const GameSettings& settings);
    bool LoadSettings(GameSettings& settings);

    void CreateProfile(const std::string& profileName);
    void DeleteProfile(const std::string& profileName);
    std::vector<std::string> GetAvailableProfiles() const;
    PlayerStats GetPlayerStats(const std::string& profileName) const;

    void SetCurrentProfile(const std::string& profileName) { currentProfile = profileName; }
    std::string GetCurrentProfile() const { return currentProfile; }

private:
    std::string GetSaveFilePath(const std::string& profileName) const;
    std::string GetSettingsFilePath() const;
    bool DirectoryExists(const std::string& path) const;
    bool CreateDirectory(const std::string& path) const;
};

#endif