#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "PhysicsEntity.h"

class Projectile : public PhysicsEntity {
private:
    float damage;
    std::string ownerType;
    uint32_t ownerId;
    float lifetime;
    float maxLifetime;

public:
    Projectile(Vector2 pos, Vector2 velocity, float damage,
               const std::string& ownerType, uint32_t ownerId);
    ~Projectile();

    void InitializePhysics(cpSpace* space) override;
    void Update(Game* game, float deltaTime) override;
    void Draw() override;
    void OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) override;

    // Геттеры
    float GetDamage() const { return damage; }
    std::string GetOwnerType() const { return ownerType; }
    uint32_t GetOwnerId() const { return ownerId; }
    bool IsExpired() const { return lifetime >= maxLifetime; }
};

#endif