//
// Created by Admin on 04.11.2025.
//
#include "Entity.h"
#include "Game.h"
#include "raymath.h"

Entity::Entity(Vector2 pos, float size, Color color)
        : position(pos),
          velocity({0, 0}),
          rotation(0.0f),
          size(size),
          color(color),
          active(true) {
}

void Entity::Update(Game* game, float deltaTime) {
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void Entity::Draw() {
    DrawCircleV(position, size, color);
}

void Entity::OnCollision(Entity* other) {
    // Базовая реализация - можно переопределить
}

Rectangle Entity::GetBoundingBox() const {
    return Rectangle{
            position.x - size,
            position.y - size,
            size * 2,
            size * 2
    };
}

bool Entity::CheckCollision(const Entity* other) const {
    float distance = Vector2Distance(position, other->position);
    return distance < (size + other->size);
}