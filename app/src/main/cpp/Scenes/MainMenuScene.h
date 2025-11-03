//
// Created by Admin on 02.11.2025.
//
#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "Scene.h"

class MainMenuScene : public Scene {
public:
    void Update(Game* game) override;
    void Draw(Game* game) override;
    void OnEnter() override;

private:
    void DrawBackground();
    void DrawTitle(Game* game);
    void DrawMenuButtons(Game* game);
};

#endif
