#ifndef ENEMY_H
#define ENEMY_H

#include "PhysicsEntity.h"
#include <memory>

class Enemy : public PhysicsEntity {
public:
    enum class Type {
        SCOUT,      // Быстрый, слабый
        SOLDIER,    // Средний
        TANK,       // Медленный, сильный
        SNIPER,     // Дальний бой
        BOSS        // Босс волны
    };

private:
    Type enemyType;
    int health;
    int maxHealth;
    int damage;
    float moveSpeed;
    float attackRange;
    float attackCooldown;
    float lastAttackTime;
    bool targetAcquired;
    Vector2 targetPosition;
    std::string behaviorState; // "patrol", "attack", "flee"

public:
    Enemy(Vector2 pos, Type type, int waveLevel);
    ~Enemy();

    void InitializePhysics(cpSpace* space) override;
    void Update(Game* game, float deltaTime) override;
    void Draw() override;
    void OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) override;
    void Patrol(float deltaTime);
    void TakeDamage(int damage);
    void Die(Game* game);
    void AcquireTarget(Vector2 target);
    void UpdateAI(Game* game, float deltaTime);
    void MoveTowardsTarget(float deltaTime);
    void Attack(Game* game);

    // Геттеры
    Type GetEnemyType() const { return enemyType; }
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }
    int GetDamage() const { return damage; }
    bool IsAlive() const { return health > 0; }

private:
    void InitializeFromType(Type type, int waveLevel);
    Color GetColorByType() const;
};

#endif