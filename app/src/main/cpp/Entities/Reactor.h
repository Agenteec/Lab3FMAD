#ifndef REACTOR_H
#define REACTOR_H

#include "PhysicsEntity.h"

class Reactor : public PhysicsEntity {
private:
    int maxHealth;
    int currentHealth;
    int upgradeLevel;
    float energyOutput;
    bool isDestroyed;
    float damageCooldown;
    float lastDamageTime;

public:
    Reactor(Vector2 pos);
    ~Reactor();

    void InitializePhysics(cpSpace* space) override;
    void Update(Game* game, float deltaTime) override;
    void Draw() override;
    void OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) override;

    void TakeDamage(int damage);
    void Repair(int amount);
    void Upgrade();
    void GenerateEnergy(Game* game);

    // Геттеры
    int GetHealth() const { return currentHealth; }
    int GetMaxHealth() const { return maxHealth; }
    int GetUpgradeLevel() const { return upgradeLevel; }
    float GetEnergyOutput() const { return energyOutput; }
    bool IsDestroyed() const { return isDestroyed; }

    float GetHealthPercentage() const {
        return static_cast<float>(currentHealth) / maxHealth;
    }
};

#endif