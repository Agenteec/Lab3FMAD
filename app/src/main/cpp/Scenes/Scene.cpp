//
// Created by Admin on 02.11.2025.
//
#include "Scene.h"
#include "../Game.h"

Font Scene::GetFont(Game* game) {
    return game->GetFont();
}

void Scene::DrawCommonElements(Game* game) {
    DrawTextEx(GetFont(game), TextFormat("FPS: %d", GetFPS()), (Vector2){20, 20}, 30, 1, GREEN);

    game->DrawBottomRightPanel();
}