//
// Created by Admin on 04.11.2025.
//
#include "Projectile.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#include "raymath.h"

Projectile::Projectile(Vector2 pos, Vector2 velocity, float damage,
                       const std::string& ownerType, uint32_t ownerId)
        : PhysicsEntity(pos, 4.0f, YELLOW),
          damage(damage),
          ownerType(ownerType),
          ownerId(ownerId),
          lifetime(0.0f),
          maxLifetime(5.0f) {

    this->velocity = velocity;

    // Разные цвета для разных владельцев
    if (ownerType == "player") {
        color = SKYBLUE;
    } else if (ownerType == "enemy") {
        color = RED;
    }
}

Projectile::~Projectile() {
}

void Projectile::InitializePhysics(cpSpace* space) {
    cpFloat mass = 0.1f; // Легкие снаряды
    cpFloat moment = cpMomentForCircle(mass, 0, size, cpvzero);
    physicsBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(physicsBody, cpv(position.x, position.y));
    cpBodySetVelocity(physicsBody, cpv(velocity.x, velocity.y));

    physicsShape = cpSpaceAddShape(space, cpCircleShapeNew(physicsBody, size, cpvzero));
    cpShapeSetFriction(physicsShape, 0.0f);
    cpShapeSetElasticity(physicsShape, 0.8f);

    // Снаряды сталкиваются со врагами, игроками и реактором в зависимости от владельца
    // FIX: Changed cpBitMask to cpBitmask
    cpBitmask collisionCategories = CollisionCategory::PROJECTILE;
    cpBitmask collisionMask = 0;

    if (ownerType == "player") {
        collisionMask = CollisionCategory::ENEMY | CollisionCategory::REACTOR;
    } else if (ownerType == "enemy") {
        collisionMask = CollisionCategory::PLAYER | CollisionCategory::REACTOR;
    }

    cpShapeSetFilter(physicsShape,
                     cpShapeFilterNew(collisionCategories, collisionMask, 0));

    cpShapeSetUserData(physicsShape, this);
    physicsInitialized = true;
}

void Projectile::Update(Game* game, float deltaTime) {
    PhysicsEntity::Update(game, deltaTime);

    lifetime += deltaTime;
    if (lifetime >= maxLifetime) {
        SetActive(false);
        return;
    }

    // Мерцание перед исчезновением
    if (lifetime > maxLifetime - 1.0f) {
        float blink = sinf(lifetime * 20.0f) * 0.5f + 0.5f;
        color.a = static_cast<unsigned char>(255 * blink);
    }
}

void Projectile::Draw() {
    // Основной круг снаряда
    DrawCircleV(position, size, color);

    // Светящийся эффект
    DrawCircleV(position, size * 0.6f, Fade(WHITE, 0.7f));

    // Хвост/след
    Vector2 tailDirection = Vector2Normalize(velocity);
    Vector2 tailStart = {
            position.x - tailDirection.x * size * 2,
            position.y - tailDirection.y * size * 2
    };

    DrawLineEx(tailStart, position, size * 0.5f, Fade(color, 0.5f));
}

void Projectile::OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) {
    // Получаем нормаль столкновения
    cpVect normal = cpArbiterGetNormal(arb);

    // Отскок
    if (physicsBody) {
        cpVect currentVel = cpBodyGetVelocity(physicsBody);
        cpVect reflect = cpvsub(currentVel, cpvmult(normal, 2.0f * cpvdot(currentVel, normal)));
        cpBodySetVelocity(physicsBody, cpvmult(reflect, 0.8f)); // Небольшое затухание
    }

    // Наносим урон в зависимости от типа цели
    if (ownerType == "player") {
        Enemy* enemy = dynamic_cast<Enemy*>(other);
        if (enemy) {
            enemy->TakeDamage(damage);
            SetActive(false); // Снаряд уничтожается при попадании
        }
    } else if (ownerType == "enemy") {
        Player* player = dynamic_cast<Player*>(other);
        Reactor* reactor = dynamic_cast<Reactor*>(other);

        if (player) {
            // Игрок получает урон
            // player->TakeDamage(damage);
            SetActive(false);
        } else if (reactor) {
            reactor->TakeDamage(damage);
            SetActive(false);
        }
    }
}