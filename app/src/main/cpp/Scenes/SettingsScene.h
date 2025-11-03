//
// Created by Admin on 02.11.2025.
//

#ifndef SETTINGS_SCENE_H
#define SETTINGS_SCENE_H

#include "Scene.h"

class SettingsScene : public Scene {
public:
    void Update(Game* game) override;
    void Draw(Game* game) override;
};

#endif