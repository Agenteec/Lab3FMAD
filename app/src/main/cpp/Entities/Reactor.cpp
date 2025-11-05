//
// Created by Admin on 04.11.2025.
//
#include "Reactor.h"
#include "../Game/Game.h"
#include "../Entities/Enemy.h"
#include <iostream>

Reactor::Reactor(Vector2 pos)
        : PhysicsEntity(pos, 40.0f, BLUE),
          maxHealth(1000),
          currentHealth(1000),
          upgradeLevel(1),
          energyOutput(10.0f),
          isDestroyed(false),
          damageCooldown(1.0f),
          lastDamageTime(0.0f) {
}

Reactor::~Reactor() {
}

void Reactor::InitializePhysics(cpSpace* space) {
    cpFloat mass = 100.0f; // Тяжелый реактор
    cpFloat moment = cpMomentForCircle(mass, 0, size, cpvzero);
    physicsBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(physicsBody, cpv(position.x, position.y));
    cpBodySetType(physicsBody, CP_BODY_TYPE_STATIC); // Неподвижный

    physicsShape = cpSpaceAddShape(space, cpCircleShapeNew(physicsBody, size, cpvzero));
    cpShapeSetFriction(physicsShape, 0.7f);
    cpShapeSetElasticity(physicsShape, 0.1f);

    cpShapeSetFilter(physicsShape,
                     cpShapeFilterNew(CollisionCategory::REACTOR,
                                      CollisionCategory::ENEMY | CollisionCategory::PROJECTILE,
                                      CollisionCategory::REACTOR));

    cpShapeSetUserData(physicsShape, this);
    physicsInitialized = true;
}

void Reactor::Update(Game* game, float deltaTime) {
    PhysicsEntity::Update(game, deltaTime);

    if (isDestroyed) {
        return;
    }

    // Генерация энергии
    GenerateEnergy(game);

    // Визуальный эффект при низком здоровье
    if (GetHealthPercentage() < 0.3f) {
        float pulse = sinf(GetTime() * 5.0f) * 0.3f + 0.7f;
        color = Fade(RED, pulse);
    }
}

void Reactor::Draw() {
    // Основной корпус реактора
    DrawCircleV(position, size, color);

    // Внутреннее ядро
    DrawCircleV(position, size * 0.6f, Fade(YELLOW, 0.8f));

    // Энергетическое поле
    DrawCircleLines(position.x, position.y, size * 1.2f, Fade(SKYBLUE, 0.5f));

    // Детали реактора
    for (int i = 0; i < 8; i++) {
        float angle = i * PI / 4 + GetTime();
        Vector2 spikePos = {
                position.x + cosf(angle) * size * 0.8f,
                position.y + sinf(angle) * size * 0.8f
        };
        DrawCircleV(spikePos, size * 0.15f, Fade(ORANGE, 0.9f));
    }

    // Индикатор уровня улучшения
    DrawText(TextFormat("Lvl %d", upgradeLevel),
             position.x - 15, position.y - size - 25, 20, WHITE);
}

void Reactor::OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) {
    Enemy* enemy = dynamic_cast<Enemy*>(other);
    if (enemy) {
        float currentTime = GetTime();
        if (currentTime - lastDamageTime >= damageCooldown) {
            TakeDamage(enemy->GetDamage());
            lastDamageTime = currentTime;
        }
    }
}

void Reactor::TakeDamage(int damage) {
    currentHealth -= damage;
    currentHealth = std::max(0, currentHealth);

    if (currentHealth <= 0) {
        isDestroyed = true;
        // Эффект разрушения
        // game->GetParticleSystem()->Emit(ParticleEffect::EXPLOSION, position, 50, RED, 3.0f);
    }

    // Визуальная обратная связь
    color = Fade(RED, 0.8f);
}

void Reactor::Repair(int amount) {
    currentHealth += amount;
    currentHealth = std::min(currentHealth, maxHealth);

    // Визуальная обратная связь
    color = Fade(GREEN, 0.8f);
}

void Reactor::Upgrade() {
    upgradeLevel++;
    maxHealth += 200;
    currentHealth = maxHealth; // Полное исцеление при улучшении
    energyOutput += 5.0f;
    size += 5.0f;

    // Обновляем физическую форму
    if (physicsShape && physicsBody) {
        cpSpace* space = cpShapeGetSpace(physicsShape);
        CleanupPhysics(space);
        InitializePhysics(space);
    }
}

void Reactor::GenerateEnergy(Game* game) {
    // Генерация ресурсов для игрока
    static float energyTimer = 0.0f;
    energyTimer += GetFrameTime();

    if (energyTimer >= 1.0f) { // Каждую секунду
        game->AddResources(static_cast<int>(energyOutput));
        energyTimer = 0.0f;
    }
}