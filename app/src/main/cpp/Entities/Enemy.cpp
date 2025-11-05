#include "Enemy.h"
#include "Game.h"
#include "Player.h"
#include "Reactor.h"
#include <algorithm>
#include "raymath.h"

Enemy::Enemy(Vector2 pos, Type type, int waveLevel)
        : PhysicsEntity(pos, 15.0f, RED),
          enemyType(type),
          health(100),
          maxHealth(100),
          damage(10),
          moveSpeed(50.0f),
          attackRange(100.0f),
          attackCooldown(1.0f),
          lastAttackTime(0.0f),
          targetAcquired(false),
          behaviorState("patrol") {

    InitializeFromType(type, waveLevel);
    color = GetColorByType();
}

Enemy::~Enemy() {
}

// NOTE: The Patrol function is now defined BEFORE UpdateAI calls it.
void Enemy::Patrol(float deltaTime) {
    // Простое патрулирование по кругу
    static float patrolAngle = 0.0f;
    patrolAngle += deltaTime * 0.5f;

    float patrolRadius = 100.0f;
    targetPosition = {
            GetScreenWidth()/2 + cosf(patrolAngle) * patrolRadius,
            GetScreenHeight()/2 + sinf(patrolAngle) * patrolRadius
    };

    MoveTowardsTarget(deltaTime);
}

// This is the SINGLE, correct version of UpdateAI
void Enemy::UpdateAI(Game* game, float deltaTime) {
    Reactor* reactor = game->GetReactor();
    if (!reactor || reactor->IsDestroyed()) {
        // Если реактор уничтожен, патрулируем
        behaviorState = "patrol";
        Patrol(deltaTime); // This will now compile correctly.
        return;
    }

    Vector2 reactorPos = reactor->GetPosition();

    // TODO: Реализовать поиск реального игрока из Game
    // For now, the default target is the reactor.
    Vector2 nearestPlayerPos = reactorPos;
    float distanceToPlayer = Vector2Distance(position, nearestPlayerPos);

    // Выбор цели: игрок ближе 150px или реактор
    if (distanceToPlayer < 150.0f) {
        AcquireTarget(nearestPlayerPos);
    } else {
        AcquireTarget(reactorPos);
    }

    if (behaviorState == "attack") {
        float distanceToTarget = Vector2Distance(position, targetPosition);

        if (distanceToTarget <= attackRange) {
            // В радиусе атаки - атаковать
            Attack(game);
        } else {
            // Двигаться к цели
            MoveTowardsTarget(deltaTime);
        }
    } else if (behaviorState == "patrol") {
        Patrol(deltaTime);
    }
}


void Enemy::InitializeFromType(Type type, int waveLevel) {
    float waveMultiplier = 1.0f + (waveLevel - 1) * 0.2f;

    switch (type) {
        case Type::SCOUT:
            size = 12.0f;
            health = 50 * waveMultiplier;
            maxHealth = health;
            damage = 5 * waveMultiplier;
            moveSpeed = 80.0f;
            attackRange = 80.0f;
            attackCooldown = 0.8f;
            break;

        case Type::SOLDIER:
            size = 15.0f;
            health = 100 * waveMultiplier;
            maxHealth = health;
            damage = 10 * waveMultiplier;
            moveSpeed = 60.0f;
            attackRange = 100.0f;
            attackCooldown = 1.0f;
            break;

        case Type::TANK:
            size = 25.0f;
            health = 300 * waveMultiplier;
            maxHealth = health;
            damage = 20 * waveMultiplier;
            moveSpeed = 30.0f;
            attackRange = 120.0f;
            attackCooldown = 1.5f;
            break;

        case Type::SNIPER:
            size = 10.0f;
            health = 60 * waveMultiplier;
            maxHealth = health;
            damage = 25 * waveMultiplier;
            moveSpeed = 40.0f;
            attackRange = 200.0f;
            attackCooldown = 2.0f;
            break;

        case Type::BOSS:
            size = 40.0f;
            health = 1000 * waveMultiplier;
            maxHealth = health;
            damage = 30 * waveMultiplier;
            moveSpeed = 25.0f;
            attackRange = 150.0f;
            attackCooldown = 1.0f;
            break;
    }
}

void Enemy::InitializePhysics(cpSpace* space) {
    cpFloat mass = size * 0.5f; // Масса пропорциональна размеру
    cpFloat moment = cpMomentForCircle(mass, 0, size, cpvzero);
    physicsBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(physicsBody, cpv(position.x, position.y));

    physicsShape = cpSpaceAddShape(space, cpCircleShapeNew(physicsBody, size, cpvzero));
    cpShapeSetFriction(physicsShape, 0.4f);
    cpShapeSetElasticity(physicsShape, 0.2f);

    cpShapeSetFilter(physicsShape,
                     cpShapeFilterNew(CollisionCategory::ENEMY,
                                      CollisionCategory::PLAYER | CollisionCategory::PROJECTILE | CollisionCategory::REACTOR,
                                      CollisionCategory::ENEMY));

    cpShapeSetUserData(physicsShape, this);
    physicsInitialized = true;
}

void Enemy::Update(Game* game, float deltaTime) {
    PhysicsEntity::Update(game, deltaTime);

    if (!IsAlive()) {
        Die(game);
        return;
    }

    UpdateAI(game, deltaTime);
}

void Enemy::Draw() {
    // Основной круг врага
    DrawCircleV(position, size, color);

    // Индикатор здоровья
    float healthPercent = static_cast<float>(health) / maxHealth;
    Color healthColor = GREEN;
    if (healthPercent < 0.3f) healthColor = RED;
    else if (healthPercent < 0.6f) healthColor = YELLOW;

    // Полоска здоровья над врагом
    float barWidth = size * 2;
    float barHeight = 4;
    Vector2 barPos = { position.x - barWidth / 2, position.y - size - 10 };

    DrawRectangle(barPos.x, barPos.y, barWidth, barHeight, GRAY);
    DrawRectangle(barPos.x, barPos.y, barWidth * healthPercent, barHeight, healthColor);

    // Визуализация радиуса атаки при наличии цели
    if (targetAcquired && behaviorState == "attack") {
        DrawCircleLines(position.x, position.y, attackRange, Fade(RED, 0.3f));
    }
}

void Enemy::OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) {
    // Обработка столкновений с другими объектами
    // Например, получение урона от снарядов игрока
}

void Enemy::TakeDamage(int damage) {
    health -= damage;
    health = std::max(0, health);

    // Визуальная обратная связь при получении урона
    color = Fade(GetColorByType(), 0.7f);
}

void Enemy::Die(Game* game) {
    SetActive(false);

    // Награда за убийство
    game->AddResources(10 * (static_cast<int>(enemyType) + 1));
    game->AddScore(25 * (static_cast<int>(enemyType) + 1));

    // Эффект смерти
    // Можно добавить частицы или анимацию
}

void Enemy::AcquireTarget(Vector2 target) {
    targetPosition = target;
    targetAcquired = true;
    behaviorState = "attack";
}

void Enemy::MoveTowardsTarget(float deltaTime) {
    if (!targetAcquired) return;

    Vector2 direction = {
            targetPosition.x - position.x,
            targetPosition.y - position.y
    };

    // Нормализация направления
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }

    // Применение силы для движения
    Vector2 force = { direction.x * moveSpeed, direction.y * moveSpeed };
    ApplyForce(force);

    // Поворот в направлении движения
    if (length > 0.1f) {
        rotation = atan2f(direction.y, direction.x);
    }
}

void Enemy::Attack(Game* game) {
    float currentTime = GetTime();
    if (currentTime - lastAttackTime >= attackCooldown) {
        Reactor* reactor = game->GetReactor();
        if (reactor && Vector2Distance(position, reactor->GetPosition()) <= attackRange) {
            reactor->TakeDamage(damage);
            lastAttackTime = currentTime;
        }
    }
}

Color Enemy::GetColorByType() const {
    switch (enemyType) {
        case Type::SCOUT: return SKYBLUE;
        case Type::SOLDIER: return RED;
        case Type::TANK: return DARKGRAY;
        case Type::SNIPER: return PURPLE;
        case Type::BOSS: return MAROON;
        default: return RED;
    }
}

