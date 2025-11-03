//
// Created by Admin on 02.11.2025.
//

#ifndef ABOUT_SCENE_H
#define ABOUT_SCENE_H

#include "Scene.h"

class AboutScene : public Scene {
public:
    void Update(Game* game) override;
    void Draw(Game* game) override;
};

#endif