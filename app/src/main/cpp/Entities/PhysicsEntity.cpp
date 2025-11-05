//
// Created by Admin on 04.11.2025.
//
#include "PhysicsEntity.h"
#include "../Game/Game.h"
#include <iostream>

PhysicsEntity::PhysicsEntity(Vector2 pos, float size, Color color)
        : Entity(pos, size, color),
          physicsBody(nullptr),
          physicsShape(nullptr),
          physicsInitialized(false) {
}

PhysicsEntity::~PhysicsEntity() {
}

void PhysicsEntity::Update(Game* game, float deltaTime) {
    if (physicsInitialized && physicsBody) {
        // Синхронизация позиции и вращения из физического движка
        cpVect physPos = cpBodyGetPosition(physicsBody);
        position = Vector2{static_cast<float>(physPos.x), static_cast<float>(physPos.y)};
        rotation = cpBodyGetAngle(physicsBody);

        // Синхронизация скорости
        cpVect vel = cpBodyGetVelocity(physicsBody);
        velocity = Vector2{static_cast<float>(vel.x), static_cast<float>(vel.y)};
    }
}

void PhysicsEntity::Draw() {
    // Базовая отрисовка - круг
    DrawCircleV(position, size, color);

    // Линия, показывающая направление
    Vector2 direction = {
            position.x + cosf(rotation) * size,
            position.y + sinf(rotation) * size
    };
    DrawLineV(position, direction, WHITE);
}

void PhysicsEntity::CleanupPhysics(cpSpace* space) {
    if (physicsShape) {
        cpSpaceRemoveShape(space, physicsShape);
        cpShapeFree(physicsShape);
        physicsShape = nullptr;
    }

    if (physicsBody) {
        cpSpaceRemoveBody(space, physicsBody);
        cpBodyFree(physicsBody);
        physicsBody = nullptr;
    }

    physicsInitialized = false;
}

Vector2 PhysicsEntity::GetPhysicsPosition() const {
    if (physicsBody) {
        cpVect pos = cpBodyGetPosition(physicsBody);
        return Vector2{static_cast<float>(pos.x), static_cast<float>(pos.y)};
    }
    return position;
}

float PhysicsEntity::GetPhysicsRotation() const {
    if (physicsBody) {
        return cpBodyGetAngle(physicsBody);
    }
    return rotation;
}

void PhysicsEntity::SetPhysicsPosition(Vector2 position) {
    if (physicsBody) {
        cpBodySetPosition(physicsBody, cpv(position.x, position.y));
    }
    this->position = position;
}

void PhysicsEntity::SetPhysicsVelocity(Vector2 velocity) {
    if (physicsBody) {
        cpBodySetVelocity(physicsBody, cpv(velocity.x, velocity.y));
    }
    this->velocity = velocity;
}

void PhysicsEntity::ApplyForce(Vector2 force) {
    if (physicsBody) {
        cpBodyApplyForceAtWorldPoint(physicsBody, cpv(force.x, force.y), cpBodyGetPosition(physicsBody));
    }
}

void PhysicsEntity::ApplyImpulse(Vector2 impulse) {
    if (physicsBody) {
        cpBodyApplyImpulseAtWorldPoint(physicsBody, cpv(impulse.x, impulse.y), cpBodyGetPosition(physicsBody));
    }
}

void PhysicsEntity::OnPhysicsCollision(PhysicsEntity* other, cpArbiter* arb) {
    // Базовая реализация - можно переопределить в дочерних классах
}

void PhysicsEntity::OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) {
    // Базовая реализация
}

void PhysicsEntity::OnPhysicsCollisionSeparate(PhysicsEntity* other, cpArbiter* arb) {
    // Базовая реализация
}