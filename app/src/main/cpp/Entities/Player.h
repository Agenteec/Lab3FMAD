#ifndef PLAYER_H
#define PLAYER_H

#include "PhysicsEntity.h"
#include <string>

class Player : public PhysicsEntity {
private:
    std::string name;
    int level;
    float experience;
    int kills;

    // Статистики
    float moveForce;
    float maxSpeed;
    float bulletDamage;
    float fireRate;
    float lastFireTime;

    // Управление
    Vector2 inputDirection;
    bool isFiring;

public:
    Player(Vector2 pos, const std::string& playerName);
    ~Player();

    void InitializePhysics(cpSpace* space) override;
    void Update(Game* game, float deltaTime) override;
    void Draw() override;
    void OnCollision(Entity* other) override;
    void OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) override;

    void HandleInput();
    void Fire(Game* game);
    void LevelUp();
    void AddExperience(float exp);

    // Геттеры
    std::string GetName() const { return name; }
    int GetLevel() const { return level; }
    float GetExperience() const { return experience; }
    int GetKills() const { return kills; }

    // Улучшения
    void UpgradeMoveForce();
    void UpgradeMaxSpeed();
    void UpgradeBulletDamage();
    void UpgradeFireRate();
    void UpgradeBodySize();

    void SetInputDirection(Vector2 dir) { inputDirection = dir; }
    void SetFiring(bool firing) { isFiring = firing; }

private:
    void UpdateMovement(float deltaTime);
};

#endif