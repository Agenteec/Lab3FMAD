#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "Scene.h"
#include "../Entities/Player.h"
#include "../Entities/Enemy.h"
#include "../Entities/Projectile.h"
#include <vector>
#include <memory>

class GameScene : public Scene {
private:
    std::unique_ptr<Player> localPlayer;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Projectile>> projectiles;

    float waveTimer;
    float spawnTimer;
    bool waveInProgress;
    int enemiesRemaining;
    int maxEnemiesPerWave;

public:
    GameScene();
    void Update(Game* game) override;
    void Draw(Game* game) override;
    void OnEnter() override;
    void OnExit() override;
    Vector2 GetRandomSpawnPosition(Game* game);
private:
    void DrawBackground();
    void DrawGameUI(Game* game);
    void DrawMinimap(Game* game);
    void UpdateNetworkGame(Game* game);
    void UpdateSinglePlayer(Game* game);
    void SpawnEnemyWave(Game* game);
    void SpawnEnemy(Game* game, Vector2 position);
    void UpdatePhysics(Game* game, float deltaTime);
    void CheckCollisions(Game* game);
    void CleanupInactiveEntities();
    void HandlePlayerInput(Game* game);
    void SpawnProjectile(Game* game, Vector2 position, Vector2 direction,
                         float damage, const std::string& ownerType, uint32_t ownerId);

    void UpdateNetworkPlayers(Game *game);
};

#endif