#ifndef UPGRADE_SCENE_H
#define UPGRADE_SCENE_H

#include "Scene.h"
#include "string"

class UpgradeScene : public Scene {
private:
    int availableResources;
    float sceneTimer;
    float maxSceneTime;

public:
    UpgradeScene();
    void Update(Game* game) override;
    void Draw(Game* game) override;
    void OnEnter() override;

private:
    void DrawUpgradeOptions(Game* game);
    void DrawTimer(Game* game);
    void ApplyUpgrade(Game* game, const std::string& upgradeType);
};

#endif