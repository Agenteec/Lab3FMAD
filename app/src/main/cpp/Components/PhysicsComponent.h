#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "raylib.h"

class PhysicsComponent {
private:
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float rotation;
    float angularVelocity;
    float mass;
    float friction;

public:
    PhysicsComponent(Vector2 startPos = {0,0}, float mass = 1.0f);

    void Update(float deltaTime);
    void ApplyForce(Vector2 force);
    void ApplyImpulse(Vector2 impulse);
    void SetVelocity(Vector2 newVelocity);
    void SetPosition(Vector2 newPosition);

    Vector2 GetPosition() const { return position; }
    Vector2 GetVelocity() const { return velocity; }
    float GetRotation() const { return rotation; }
    float GetMass() const { return mass; }
};

#endif