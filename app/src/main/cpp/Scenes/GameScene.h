//
// Created by Admin on 02.11.2025.
//

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "Scene.h"

class GameScene : public Scene {
public:
    void Update(Game* game) override;
    void Draw(Game* game) override;

private:
    void DrawBackground();
    void DrawGameUI(Game* game);
};

#endif
