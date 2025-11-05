//
// Created by Admin on 05.11.2025.
//

#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H
class GameSettings {
public:
    float musicVolume = 0.7f;
    float soundVolume = 0.8f;
    bool fullscreen = true;
    int language = 0; // 0 - русский, 1 - английский
    std::string playerName = "Player";
    bool vsync = true;
    int targetFPS = 60;
};
#endif
