//
// Created by Admin on 04.11.2025.
//
#include "Player.h"
#include "Game.h"
#include "Projectile.h"
#include <iostream>
#include "raymath.h"
#include "GameScene.h"

Player::Player(Vector2 pos, const std::string& playerName)
        : PhysicsEntity(pos, 20.0f, BLUE),
          name(playerName),
          level(1),
          experience(0.0f),
          kills(0),
          moveForce(500.0f),
          maxSpeed(200.0f),
          bulletDamage(25.0f),
          fireRate(2.0f),
          lastFireTime(0.0f),
          inputDirection({0, 0}),
          isFiring(false) {
}

Player::~Player() {
}
//void Player::UpgradeMoveForce() {
//    moveForce += 100.0f;
//    maxSpeed += 25.0f;
//}


void Player::UpgradeFireRate() {
    fireRate += 0.5f;
}

void Player::UpgradeBodySize() {
    size += 2.0f;

    // Обновляем физическую форму
    if (physicsShape && physicsBody) {
        cpSpace* space = cpShapeGetSpace(physicsShape);
        CleanupPhysics(space);
        InitializePhysics(space);
    }
}
void Player::InitializePhysics(cpSpace* space) {
    cpFloat mass = 2.0f;
    cpFloat moment = cpMomentForCircle(mass, 0, size, cpvzero);
    physicsBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(physicsBody, cpv(position.x, position.y));

    physicsShape = cpSpaceAddShape(space, cpCircleShapeNew(physicsBody, size, cpvzero));
    cpShapeSetFriction(physicsShape, 0.4f);
    cpShapeSetElasticity(physicsShape, 0.3f);

    cpShapeSetFilter(physicsShape,
                     cpShapeFilterNew(CollisionCategory::PLAYER,
                                      CollisionCategory::ENEMY | CollisionCategory::PROJECTILE | CollisionCategory::REACTOR,
                                      CollisionCategory::PLAYER));

    cpShapeSetUserData(physicsShape, this);
    physicsInitialized = true;
}

void Player::Update(Game* game, float deltaTime) {
    PhysicsEntity::Update(game, deltaTime);

    HandleInput();
    UpdateMovement(deltaTime);

    if (isFiring) {
        Fire(game);
    }
}

void Player::Draw() {
    // Основной корпус
    DrawCircleV(position, size, color);

    // Направление корабля
    Vector2 direction = {
            position.x + cosf(rotation) * size * 1.2f,
            position.y + sinf(rotation) * size * 1.2f
    };
    DrawLineEx(position, direction, 3.0f, WHITE);

    // Двигатели
    Vector2 leftEngine = {
            position.x + cosf(rotation + 2.5f) * size * 0.8f,
            position.y + sinf(rotation + 2.5f) * size * 0.8f
    };
    Vector2 rightEngine = {
            position.x + cosf(rotation - 2.5f) * size * 0.8f,
            position.y + sinf(rotation - 2.5f) * size * 0.8f
    };

    // Эффект двигателей при движении
    if (Vector2Length(inputDirection)  > 0.1f) {
        float enginePower = sinf(GetTime() * 20.0f) * 0.5f + 0.5f;
        DrawCircleV(leftEngine, size * 0.3f, Fade(ORANGE, enginePower));
        DrawCircleV(rightEngine, size * 0.3f, Fade(ORANGE, enginePower));
    }

    // Имя и уровень игрока
    DrawText(name.c_str(), position.x - 30, position.y - size - 30, 20, WHITE);
    DrawText(TextFormat("Lvl %d", level), position.x - 15, position.y - size - 50, 16, GREEN);
}

void Player::OnCollision(Entity* other) {
    // Обработка столкновений (устаревший метод)
}

void Player::OnPhysicsCollisionBegin(PhysicsEntity* other, cpArbiter* arb) {
    Enemy* enemy = dynamic_cast<Enemy*>(other);
    if (enemy) {
        // Столкновение с врагом наносит урон
        // TakeDamage(10);
    }
}

void Player::HandleInput() {
    inputDirection = {0, 0};

    // Управление с клавиатуры
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) inputDirection.y -= 1;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) inputDirection.y += 1;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) inputDirection.x -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) inputDirection.x += 1;

    // Стрельба
    isFiring = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    // Наведение на мышь
    Vector2 mousePos = GetMousePosition();
    Vector2 directionToMouse = {
            mousePos.x - position.x,
            mousePos.y - position.y
    };

    if (Vector2Length(directionToMouse) > 0.1f) {
        rotation = atan2f(directionToMouse.y, directionToMouse.x);
    }
}

void Player::UpdateMovement(float deltaTime) {
    if (Vector2Length(inputDirection) > 0.1f) {
        // Нормализация направления
        Vector2 normalizedDir = Vector2Normalize(inputDirection);

        // Применение силы
        Vector2 force = {
                normalizedDir.x * moveForce,
                normalizedDir.y * moveForce
        };
        ApplyForce(force);

        // Ограничение максимальной скорости
        if (physicsBody) {
            cpVect currentVel = cpBodyGetVelocity(physicsBody);
            float currentSpeed = cpvlength(currentVel);

            if (currentSpeed > maxSpeed) {
                cpVect limitedVel = cpvmult(cpvnormalize(currentVel), maxSpeed);
                cpBodySetVelocity(physicsBody, limitedVel);
            }
        }
    }
}

void Player::Fire(Game* game) {
    float currentTime = GetTime();
    if (currentTime - lastFireTime < 1.0f / fireRate) {
        return;
    }

    lastFireTime = currentTime;

    // Создание снаряда
    Vector2 bulletDirection = {
            cosf(rotation),
            sinf(rotation)
    };

    Vector2 bulletSpawn = {
            position.x + bulletDirection.x * (size + 5.0f),
            position.y + bulletDirection.y * (size + 5.0f)
    };

    Vector2 bulletVelocity = {
            bulletDirection.x * 400.0f,
            bulletDirection.y * 400.0f
    };

    // Добавляем текущую скорость игрока к скорости снаряда
    if (physicsBody) {
        cpVect playerVel = cpBodyGetVelocity(physicsBody);
        bulletVelocity.x += playerVel.x;
        bulletVelocity.y += playerVel.y;
    }

    // Создаем снаряд
    auto projectile = std::make_unique<Projectile>(bulletSpawn, bulletVelocity, bulletDamage, "player", 0);

    // Добавляем в игровую сцену
    GameScene* gameScene = dynamic_cast<GameScene*>(game->GetCurrentScene());
    if (gameScene) {
        // gameScene->AddProjectile(std::move(projectile));
    }

    // Отправка по сети
    NetworkManager* network = game->GetNetworkManager();
    if (network && network->IsConnected()) {
        network->SendProjectileCreate(*projectile);
    }
}

void Player::LevelUp() {
    level++;
    experience = 0.0f;

    // Улучшение характеристик
    moveForce += 50.0f;
    maxSpeed += 10.0f;
    bulletDamage += 5.0f;
    fireRate += 0.2f;

    std::cout << name << " reached level " << level << "!" << std::endl;
}

void Player::AddExperience(float exp) {
    experience += exp;

    // Проверка уровня
    float expRequired = level * 100.0f;
    if (experience >= expRequired) {
        LevelUp();
    }
}

void Player::UpgradeMoveForce() {
    moveForce += 100.0f;
}

void Player::UpgradeMaxSpeed() {
    maxSpeed += 25.0f;
}

void Player::UpgradeBulletDamage() {
    bulletDamage += 10.0f;
}

