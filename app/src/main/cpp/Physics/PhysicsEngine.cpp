#include "PhysicsEngine.h"
#include "PhysicsEntity.h"
#include <iostream>

PhysicsEngine::PhysicsEngine() {
    space = cpSpaceNew();
    cpSpaceSetIterations(space, 10);
    cpSpaceSetGravity(space, cpv(0, 0));
    cpSpaceSetDamping(space, 0.5f);

    // Get the default collision handler
    cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(space);

    // Assign your callback functions to the handler
    handler->beginFunc = &PhysicsEngine::CollisionBegin;
    handler->preSolveFunc = &PhysicsEngine::CollisionPreSolve;
    handler->postSolveFunc = &PhysicsEngine::CollisionPostSolve;
    handler->separateFunc = &PhysicsEngine::CollisionSeparate;

    // Pass 'this' (the PhysicsEngine instance) as user data
    handler->userData = this;
}

PhysicsEngine::~PhysicsEngine() {
    // Очистка всех entities
    entities.clear();

    // Очистка пространства Chipmunk
    if (space) {
        cpSpaceFree(space);
        space = nullptr;
    }
}

void PhysicsEngine::Update(float deltaTime) {
    // Обновление физики с фиксированным временным шагом
    cpSpaceStep(space, deltaTime);

    // Обновление всех entities
    for (auto& entity : entities) {
        if (entity) {
            // Позиция и вращение синхронизируются в PhysicsEntity::Update
        }
    }

    // Очистка уничтоженных entities
    CleanupDestroyedEntities();
}

cpBool PhysicsEngine::CollisionBegin(cpArbiter* arb, cpSpace* space, void* data) {
    PhysicsEngine* engine = static_cast<PhysicsEngine*>(data);
    cpShape* shapeA, *shapeB;
    cpArbiterGetShapes(arb, &shapeA, &shapeB);

    PhysicsEntity* entityA = static_cast<PhysicsEntity*>(cpShapeGetUserData(shapeA));
    PhysicsEntity* entityB = static_cast<PhysicsEntity*>(cpShapeGetUserData(shapeB));

    if (entityA && entityB) {
        entityA->OnPhysicsCollisionBegin(entityB, arb);
        entityB->OnPhysicsCollisionBegin(entityA, arb);
    }

    return cpTrue; // Разрешить столкновение
}
void PhysicsEngine::SetWorldBounds(double width, double height) {
    // Your implementation logic here
    // For example:
    // m_worldWidth = width;
    // m_worldHeight = height;
}
cpBool PhysicsEngine::CollisionPreSolve(cpArbiter *arb, cpSpace *space, void *data) {
    // Your implementation logic goes here.
    // For example, you might want to extract the shapes, check their properties,
    // and decide if the collision should be ignored.

    // Return 1 to process the collision, or 0 to ignore it.
    return cpTrue;
}

void PhysicsEngine::CollisionPostSolve(cpArbiter *arb, cpSpace *space, void *data) {

}
// Реализация остальных коллбэков и методов PhysicsEngine...
void PhysicsEngine::CollisionSeparate(cpArbiter *arb, cpSpace *space, void *data) {

}

void PhysicsEngine::CleanupDestroyedEntities() {

}
