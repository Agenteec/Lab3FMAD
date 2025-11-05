#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent(Vector2 startPos, float mass)
        : position(startPos), velocity({0,0}), acceleration({0,0}),
          rotation(0), angularVelocity(0), mass(mass), friction(0.98f) {
}

void PhysicsComponent::Update(float deltaTime) {
    // Обновление скорости
    velocity.x += acceleration.x * deltaTime;
    velocity.y += acceleration.y * deltaTime;

    // Применение трения
    velocity.x *= friction;
    velocity.y *= friction;

    // Обновление позиции
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // Обновление вращения
    rotation += angularVelocity * deltaTime;

    // Сброс ускорения
    acceleration = {0, 0};
}

void PhysicsComponent::ApplyForce(Vector2 force) {
    acceleration.x += force.x / mass;
    acceleration.y += force.y / mass;
}

void PhysicsComponent::ApplyImpulse(Vector2 impulse) {
    velocity.x += impulse.x / mass;
    velocity.y += impulse.y / mass;
}

void PhysicsComponent::SetVelocity(Vector2 newVelocity) {
    velocity = newVelocity;
}

void PhysicsComponent::SetPosition(Vector2 newPosition) {
    position = newPosition;
}