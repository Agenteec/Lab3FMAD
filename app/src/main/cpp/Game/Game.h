#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "../raygui.h"
#include <memory>
#include <vector>
#include "NetworkManager.h"
#include "Reactor.h"
#include "PhysicsEngine.h"
#include "../ParticleSystem.h"
#include "../AudioManager.h"
#include "../SaveSystem.h"
#include "../GameSettings.h"


enum GameState {
    MAIN_MENU,
    LOBBY,
    IN_GAME,
    UPGRADE,
    SETTINGS,
    ABOUT,
    PROFILE_SELECTION
};
class SaveSystem;
class Scene;

class Game {
private:
    GameState currentState;
    std::unique_ptr<Scene> currentScene;
    Font gameFont;
    GameSettings settings;

    // Системы
    std::unique_ptr<NetworkManager> networkManager;
    std::unique_ptr<PhysicsEngine> physicsEngine;
    std::unique_ptr<ParticleSystem> particleSystem;
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<SaveSystem> saveSystem;
    std::unique_ptr<Reactor> reactor;

    // Игровые данные
    int currentWave = 1;
    int resources = 0;
    int playerScore = 0;
    bool isHost = false;
    float gameTime = 0.0f;
    bool resourcesLoaded = false;

    void LoadResources();
    void UnloadResources();
    void ApplyUIStyle();
    static int* GetCompleteCodepoints(int* count);
    void InitializeNetwork();
    void UpdateNetwork(float deltaTime);

public:
    Game();
    ~Game();

    void Run();
    void ChangeState(GameState newState);
    Scene* GetCurrentScene() { return currentScene.get(); }

    // Геттеры систем
    const Font& GetFont() const { return gameFont; }
    GameSettings& GetSettings() { return settings; }
    NetworkManager* GetNetworkManager() { return networkManager.get(); }
    PhysicsEngine* GetPhysicsEngine() { return physicsEngine.get(); }
    ParticleSystem* GetParticleSystem() { return particleSystem.get(); }
    AudioManager* GetAudioManager() { return audioManager.get(); }
    SaveSystem* GetSaveSystem() { return saveSystem.get(); }
    Reactor* GetReactor() { return reactor.get(); }

    // Геттеры и сеттеры игровых данных
    int GetCurrentWave() const { return currentWave; }
    void SetCurrentWave(int wave) { currentWave = wave; }
    int GetResources() const { return resources; }
    void AddResources(int amount) { resources += amount; }
    bool SpendResources(int amount);
    int GetPlayerScore() const { return playerScore; }
    void AddScore(int points) { playerScore += points; }
    bool IsHost() const { return isHost; }
    void SetHost(bool host) { isHost = host; }
    float GetGameTime() const { return gameTime; }
    void UpdateGameTime(float deltaTime) { gameTime += deltaTime; }

    void DrawBottomRightPanel();
    void StartGameSession();
    void EndGameSession(bool victory);
    void StartNetworkGame(bool asHost);
    void SyncGameState();

    // Коллбэки для сети
    void OnPlayerJoin(uint32_t playerId, const std::string& name);
    void OnPlayerLeave(uint32_t playerId);
    void OnPlayerReady(uint32_t playerId, bool ready);
    void OnGameStart();
};

#endif