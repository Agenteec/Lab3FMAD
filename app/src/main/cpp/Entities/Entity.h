//
// Created by Admin on 03.11.2025.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "../Components/PhysicsComponent.h"
#include "../Components/CombatComponent.h"
#include <memory>

class Game;

class Entity {
protected:
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float size;
    Color color;
    bool active;
    std::unique_ptr<PhysicsComponent> physics;
    std::unique_ptr<CombatComponent> combat;

public:
    Entity(Vector2 pos, float size, Color color);
    virtual ~Entity() = default;

    virtual void Update(Game* game, float deltaTime);
    virtual void Draw();
    virtual void OnCollision(Entity* other);

    // Геттеры и сеттеры
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }
    Vector2 GetVelocity() const { return velocity; }
    void SetVelocity(Vector2 vel) { velocity = vel; }
    float GetRotation() const { return rotation; }
    void SetRotation(float rot) { rotation = rot; }
    float GetSize() const { return size; }
    bool IsActive() const { return active; }
    void SetActive(bool act) { active = act; }

    PhysicsComponent* GetPhysics() { return physics.get(); }
    CombatComponent* GetCombat() { return combat.get(); }

    Rectangle GetBoundingBox() const;
    bool CheckCollision(const Entity* other) const;
};

#endif
