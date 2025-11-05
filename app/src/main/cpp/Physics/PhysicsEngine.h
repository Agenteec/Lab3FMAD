#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "../Chipmunk2D/include/chipmunk/chipmunk.h"
#include <vector>
#include <functional>
#include <memory>

class PhysicsEntity;

struct CollisionCategory {
    static const cpBitmask PLAYER = 1 << 0;
    static const cpBitmask ENEMY = 1 << 1;
    static const cpBitmask PROJECTILE = 1 << 2;
    static const cpBitmask REACTOR = 1 << 3;
    static const cpBitmask WALL = 1 << 4;
    static const cpBitmask PICKUP = 1 << 5;
};

class PhysicsEngine {
private:
    cpSpace* space;
    std::vector<std::unique_ptr<PhysicsEntity>> entities;

    static cpBool CollisionBegin(cpArbiter* arb, cpSpace* space, void* data);
    static cpBool CollisionPreSolve(cpArbiter* arb, cpSpace* space, void* data);
    static void CollisionPostSolve(cpArbiter* arb, cpSpace* space, void* data);
    static void CollisionSeparate(cpArbiter* arb, cpSpace* space, void* data);

public:
    PhysicsEngine();
    ~PhysicsEngine();

    void Update(float deltaTime);
    cpSpace* GetSpace() const { return space; }

    // Методы для создания физических тел
    cpBody* CreateDynamicBody(cpFloat mass, cpFloat moment);
    cpBody* CreateKinematicBody();
    cpBody* CreateStaticBody();

    // Методы для создания форм
    cpShape* CreateCircleShape(cpBody* body, cpFloat radius, cpVect offset);
    cpShape* CreateBoxShape(cpBody* body, cpFloat width, cpFloat height);
    cpShape* CreatePolygonShape(cpBody* body, int count, const cpVect* verts, cpVect offset);

    void AddEntity(std::unique_ptr<PhysicsEntity> entity);
    void RemoveEntity(PhysicsEntity* entity);
    void CleanupDestroyedEntities();

    // Установка границ мира
    void SetWorldBounds(cpFloat width, cpFloat height);
};

#endif