//
// Created by Admin on 02.11.2025.
//

#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "../raygui.h"
class Game;

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Update(Game* game) = 0;
    virtual void Draw(Game* game) = 0;
    virtual void OnEnter() {}
    virtual void OnExit() {}

protected:
    Font GetFont(Game* game);
    void DrawCommonElements(Game* game);
};

#endif