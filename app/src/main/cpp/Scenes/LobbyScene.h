#ifndef LOBBY_SCENE_H
#define LOBBY_SCENE_H

#include "Scene.h"
#include <vector>
#include <string>

struct LobbyPlayer {
    std::string name;
    bool isReady;
    uint32_t id;
};

class LobbyScene : public Scene {
private:
    std::vector<LobbyPlayer> players;
    bool isReady;
    float countdownTimer;
    bool countdownActive;

public:
    LobbyScene();
    void Update(Game* game) override;
    void Draw(Game* game) override;
    void OnEnter() override;
    void OnExit() override;

private:
    void DrawPlayerList(Game* game);
    void DrawReadyButton(Game* game);
    void DrawCountdown(Game* game);
    void StartGameCountdown(Game* game);
};

#endif