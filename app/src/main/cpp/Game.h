#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raygui.h"
#include <memory>

struct GameSettings {
    float musicVolume = 0.7f;
    float soundVolume = 0.8f;
    bool fullscreen = true;
    int language = 0; // 0 - русский, 1 - английский
};

enum GameState {
    MAIN_MENU,
    IN_GAME,
    SETTINGS,
    ABOUT
};

class Scene;

class Game {
private:
    GameState currentState;
    std::unique_ptr<Scene> currentScene;
    Font gameFont;
    GameSettings settings;
    int gameCounter = 0;
    bool resourcesLoaded = false;

    void LoadResources();
    void UnloadResources();
    void ApplyUIStyle();
    static int* GetCompleteCodepoints(int* count);

public:
    Game();
    ~Game();

    void Run();
    void ChangeState(GameState newState);

    const Font& GetFont() const {

        return gameFont;
    }

    GameSettings& GetSettings() { return settings; }
    int GetCounter() const { return gameCounter; }
    void IncrementCounter() { gameCounter++; }

    void DrawBottomRightPanel();
};

#endif