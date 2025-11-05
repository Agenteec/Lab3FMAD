#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include "Entity.h"
#include "../Chipmunk2D/include/chipmunk/chipmunk.h"

class PhysicsEntity : public Entity {
protected:
    cpBody* physicsBody;
    cpShape* physicsShape;
    bool physicsInitialized;

public:
    PhysicsEntity(Vector2 pos, float size, Color color);
    virtual ~PhysicsEntity();

    virtual void Update(Game* game, float deltaTime) override;
    virtual void Draw() override;

    // Физические методы
    virtual void InitializePhysics(cpSpace* space) = 0;
    virtual void CleanupPhysics(cpSpace* space);

    // Геттеры для физических свойств
    cpBody* GetPhysicsBody() const { return physicsBody; }
    cpShape* GetPhysicsShape() const { return physicsShape; }
    Vector2 GetPhysicsPosition() const;
    float GetPhysicsRotation() const;

    // Сеттеры для физических свойств
    void SetPhysicsPosition(Vector2 position);
    void SetPhysicsVelocity(Vector2 velocity);
    void ApplyForce(Vector2 force);
    void ApplyImpulse(Vector2 impulse);

    // Виртуальные методы для обработки столкновений
    virtual void OnPhysicsCollision(PhysicsEntity* other, cpArbiter* arb);
    virtual void OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb);
    virtual void OnPhysicsCollisionSeparate(PhysicsEntity* other, cpArbiter* arb);
};

#endif