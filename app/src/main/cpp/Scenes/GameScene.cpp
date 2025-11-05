#include "GameScene.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Reactor.h"
#include <algorithm>
#include "iostream"

GameScene::GameScene()
        : waveTimer(0.0f),
          spawnTimer(0.0f),
          waveInProgress(false),
          enemiesRemaining(0),
          maxEnemiesPerWave(5) {
}
// Добавляем в GameScene.cpp
void GameScene::UpdateNetworkPlayers(Game* game) {
    NetworkManager* network = game->GetNetworkManager();
    if (!network) return;

    // Обновляем сетевых игроков
    auto networkPlayers = network->GetConnectedPlayers();
    for (const auto& pair : networkPlayers) {
        const NetworkPlayer& netPlayer = pair.second;

        // Пропускаем локального игрока
        if (netPlayer.id == network->GetLocalPlayerId()) continue;

        // Создаем или обновляем сетевого игрока
        // (нужно реализовать систему сетевых сущностей)
    }

    // Обновляем сетевых врагов
    auto networkEnemies = network->GetNetworkEnemies();
    for (const auto& pair : networkEnemies) {
        const NetworkEnemy& netEnemy = pair.second;

        // Создаем или обновляем сетевого врага
        // (нужно реализовать систему сетевых сущностей)
    }

    // Обновляем сетевые снаряды
    auto networkProjectiles = network->GetNetworkProjectiles();
    for (const auto& pair : networkProjectiles) {
        const NetworkProjectile& netProjectile = pair.second;

        // Создаем или обновляем сетевой снаряд
        // (нужно реализовать систему сетевых сущностей)
    }
}

void GameScene::UpdateNetworkGame(Game* game) {
    float deltaTime = GetFrameTime();
    game->UpdateGameTime(deltaTime);

    NetworkManager* network = game->GetNetworkManager();

    // Обновление физики
    UpdatePhysics(game, deltaTime);

    // Обработка ввода для локального игрока
    HandlePlayerInput(game);

    // Обновление сетевых объектов
    UpdateNetworkPlayers(game);

    // Синхронизация с сетью
    if (localPlayer) {
        network->SendPlayerUpdate(*localPlayer);
    }

    CheckCollisions(game);
    CleanupInactiveEntities();
}


void GameScene::Update(Game* game) {
    NetworkManager* network = game->GetNetworkManager();

    if (network && network->IsConnected()) {
        UpdateNetworkGame(game);
    } else {
        UpdateSinglePlayer(game);
    }
}

void GameScene::UpdateSinglePlayer(Game* game) {
    float deltaTime = GetFrameTime();
    game->UpdateGameTime(deltaTime);

    // Обновление физики
    UpdatePhysics(game, deltaTime);

    // Обработка ввода
    HandlePlayerInput(game);

    // Логика волн
    if (!waveInProgress) {
        waveTimer += deltaTime;
        if (waveTimer >= 5.0f) { // 5 секунд между волнами
            SpawnEnemyWave(game);
            waveTimer = 0.0f;
        }
    } else {
        spawnTimer += deltaTime;
        if (spawnTimer >= 2.0f && enemies.size() < maxEnemiesPerWave) {
            SpawnEnemy(game, GetRandomSpawnPosition(game));
            spawnTimer = 0.0f;
        }

        // Проверка завершения волны
        if (enemiesRemaining <= 0 && enemies.empty()) {
            waveInProgress = false;
            game->SetCurrentWave(game->GetCurrentWave() + 1);
            game->AddResources(50); // Награда за волну

            // Переход к улучшениям после каждой 3-й волны
            if (game->GetCurrentWave() % 3 == 0) {
                game->ChangeState(UPGRADE);
            }
        }
    }

    // Обновление врагов
    for (auto& enemy : enemies) {
        if (enemy->IsActive()) {
            enemy->Update(game, deltaTime);
        }
    }

    // Обновление снарядов
    for (auto& projectile : projectiles) {
        if (projectile->IsActive()) {
            projectile->Update(game, deltaTime);
        }
    }

    // Обновление реактора
    if (game->GetReactor()) {
        game->GetReactor()->Update(game, deltaTime);
    }

    CheckCollisions(game);
    CleanupInactiveEntities();
}

void GameScene::Draw(Game* game) {
    DrawBackground();
    DrawGameUI(game);
    DrawMinimap(game);
    DrawCommonElements(game);
}

void GameScene::OnEnter() {
    // Инициализация игровой сессии
    waveInProgress = false;
    waveTimer = 0.0f;
    spawnTimer = 0.0f;
    enemiesRemaining = 0;

    // Очистка предыдущих entities
    enemies.clear();
    projectiles.clear();
}

void GameScene::OnExit() {
    // Сохранение прогресса и очистка
    enemies.clear();
    projectiles.clear();
}

void GameScene::DrawBackground() {
    ClearBackground((Color){10, 10, 20, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    // Звездное небо
    for (int i = 0; i < 200; i++) {
        int x = static_cast<int>((i * 137) % width);
        int y = static_cast<int>((i * 237) % height);
        int size = (i % 3) + 1;
        float brightness = (i % 100) / 100.0f;

        Color starColor = Fade(WHITE, brightness);
        DrawCircle(x, y, size, starColor);
    }

    // Туманности
    DrawCircle(width * 0.2f, height * 0.3f, 120, (Color){100, 50, 150, 30});
    DrawCircle(width * 0.8f, height * 0.7f, 80, (Color){50, 100, 150, 40});
}

void GameScene::DrawGameUI(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    // Верхняя панель информации
    DrawRectangle(0, 0, width, 60, (Color){0, 0, 0, 180});

    // Волна и таймер
    const char* waveText = TextFormat("Волна: %d", game->GetCurrentWave());
    DrawTextEx(font, waveText, (Vector2){20, 20}, 30, 1, WHITE);

    if (waveInProgress) {
        DrawTextEx(font, TextFormat("Врагов: %d", enemiesRemaining), (Vector2){200.f, 20.f}, 30.f, 1.f, RED);
    } else {
        float countdown = 5.0f - waveTimer;
        DrawTextEx(font, TextFormat("Следующая волна: %.1f", countdown), (Vector2){200.f, 20.f}, 30.f, 1.f, YELLOW);
    }

    // Ресурсы и счет
    DrawTextEx(font, TextFormat("Ресурсы: %d", game->GetResources()), (Vector2){width - 200.f, 20.f}, 30.f, 1.f, GREEN);
    DrawTextEx(font, TextFormat("Счет: %d", game->GetPlayerScore()), (Vector2){width - 400.f, 20.f}, 30.f, 1.f, WHITE);

    // Здоровье реактора
    if (game->GetReactor()) {
        float reactorHealth = game->GetReactor()->GetHealthPercentage();
        const char* reactorText = TextFormat("Реактор: %.0f%%", reactorHealth * 100.f);
        Color reactorColor = reactorHealth > 0.6f ? GREEN : reactorHealth > 0.3f ? YELLOW : RED;
        DrawTextEx(font, reactorText, (Vector2){width / 2.f - 80, 20.f}, 30.f, 1.f, reactorColor);
    }

    // Кнопка выхода
    if (GuiButton((Rectangle){width - 150.f, height - 80.f, 130.f, 50.f}, "В МЕНЮ")) {
        game->ChangeState(MAIN_MENU);
    }
}

void GameScene::DrawMinimap(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    float minimapSize = 150.0f;
    float minimapX = width - minimapSize - 20;
    float minimapY = height - minimapSize - 20;

    // Фон миникарты
    DrawRectangleRounded((Rectangle){minimapX, minimapY, minimapSize, minimapSize}, 0.2f, 10, (Color){0, 0, 0, 180});
    DrawRectangleRoundedLines((Rectangle){minimapX, minimapY, minimapSize, minimapSize}, 0.2f, 10, WHITE);

    // Реактор на миникарте
    if (game->GetReactor()) {
        Vector2 reactorPos = game->GetReactor()->GetPosition();
        float mapX = minimapX + (reactorPos.x / width) * minimapSize;
        float mapY = minimapY + (reactorPos.y / height) * minimapSize;
        DrawCircle(mapX, mapY, 5, BLUE);
    }

    // Игрок на миникарте
    if (localPlayer) {
        Vector2 playerPos = localPlayer->GetPosition();
        float mapX = minimapX + (playerPos.x / width) * minimapSize;
        float mapY = minimapY + (playerPos.y / height) * minimapSize;
        DrawCircle(mapX, mapY, 3, GREEN);
    }

    // Враги на миникарте
    for (const auto& enemy : enemies) {
        if (enemy->IsActive()) {
            Vector2 enemyPos = enemy->GetPosition();
            float mapX = minimapX + (enemyPos.x / width) * minimapSize;
            float mapY = minimapY + (enemyPos.y / height) * minimapSize;
            DrawCircle(mapX, mapY, 2, RED);
        }
    }
}

void GameScene::SpawnEnemyWave(Game* game) {
    waveInProgress = true;
    int waveNumber = game->GetCurrentWave();

    // Увеличиваем сложность с каждой волной
    maxEnemiesPerWave = 5 + waveNumber * 2;
    enemiesRemaining = maxEnemiesPerWave;

    std::cout << "Starting wave " << waveNumber << " with " << maxEnemiesPerWave << " enemies" << std::endl;

    // Спавн первых врагов
    for (int i = 0; i < 3 && i < maxEnemiesPerWave; i++) {
        SpawnEnemy(game, GetRandomSpawnPosition(game));
    }
}

Vector2 GameScene::GetRandomSpawnPosition(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    // Спавн по краям карты
    int side = GetRandomValue(0, 3);
    Vector2 spawnPos = {0, 0};

    switch (side) {
        case 0: // Верх
            spawnPos = {(float)GetRandomValue(0, width), -50.0f};
            break;
        case 1: // Право
            spawnPos = {(float)width + 50.0f, (float)GetRandomValue(0, height)};
            break;
        case 2: // Низ
            spawnPos = {(float)GetRandomValue(0, width), (float)height + 50.0f};
            break;
        case 3: // Лево
            spawnPos = {-50.0f, (float)GetRandomValue(0, height)};
            break;
    }

    return spawnPos;
}

void GameScene::SpawnEnemy(Game* game, Vector2 position) {
    int waveNumber = game->GetCurrentWave();

    // Выбор типа врага в зависимости от волны
    Enemy::Type enemyType = Enemy::Type::SCOUT;

    if (waveNumber >= 3 && GetRandomValue(0, 100) < 30) {
        enemyType = Enemy::Type::SOLDIER;
    }
    if (waveNumber >= 5 && GetRandomValue(0, 100) < 20) {
        enemyType = Enemy::Type::TANK;
    }
    if (waveNumber >= 8 && GetRandomValue(0, 100) < 10) {
        enemyType = Enemy::Type::SNIPER;
    }
    if (waveNumber % 5 == 0 && enemiesRemaining == 1) {
        enemyType = Enemy::Type::BOSS; // Босс в конце каждой 5-й волны
    }

    auto enemy = std::make_unique<Enemy>(position, enemyType, waveNumber);

    // Инициализация физики
    enemy->InitializePhysics(game->GetPhysicsEngine()->GetSpace());

    enemies.push_back(std::move(enemy));
    enemiesRemaining--;
}

void GameScene::UpdatePhysics(Game* game, float deltaTime) {
    if (game->GetPhysicsEngine()) {
        game->GetPhysicsEngine()->Update(deltaTime);
    }
}

void GameScene::CheckCollisions(Game* game) {
    // Chipmunk автоматически обрабатывает столкновения через коллбэки
    // Здесь можно добавить дополнительную логику
}

void GameScene::CleanupInactiveEntities() {
    // Очистка неактивных врагов
    enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                           [](const std::unique_ptr<Enemy>& enemy) { return !enemy->IsActive(); }),
            enemies.end()
    );

    // Очистка неактивных снарядов
    projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                           [](const std::unique_ptr<Projectile>& projectile) { return !projectile->IsActive(); }),
            projectiles.end()
    );
}

void GameScene::HandlePlayerInput(Game* game) {
    // Обработка ввода уже реализована в Player::HandleInput()
    // Этот метод может использоваться для дополнительной обработки
}

void GameScene::SpawnProjectile(Game* game, Vector2 position, Vector2 direction,
                                float damage, const std::string& ownerType, uint32_t ownerId) {
    Vector2 velocity = {
            direction.x * 400.0f,
            direction.y * 400.0f
    };

    auto projectile = std::make_unique<Projectile>(position, velocity, damage, ownerType, ownerId);
    projectile->InitializePhysics(game->GetPhysicsEngine()->GetSpace());

    projectiles.push_back(std::move(projectile));
}


/*void GameScene::UpdateNetworkPlayers(Game* game) {
    NetworkManager* network = game->GetNetworkManager();
    if (!network) return;

    // Обновляем сетевых игроков
    auto networkPlayers = network->GetConnectedPlayers();
    for (const auto& pair : networkPlayers) {
        const NetworkPlayer& netPlayer = pair.second;

        // Пропускаем локального игрока
        if (netPlayer.id == network->GetLocalPlayerId()) continue;

        // Создаем или обновляем сетевого игрока
        // (нужно реализовать систему сетевых сущностей)
    }

    // Обновляем сетевых врагов
    auto networkEnemies = network->GetNetworkEnemies();
    for (const auto& pair : networkEnemies) {
        const NetworkEnemy& netEnemy = pair.second;

        // Создаем или обновляем сетевого врага
        // (нужно реализовать систему сетевых сущностей)
    }

    // Обновляем сетевые снаряды
    auto networkProjectiles = network->GetNetworkProjectiles();
    for (const auto& pair : networkProjectiles) {
        const NetworkProjectile& netProjectile = pair.second;

        // Создаем или обновляем сетевой снаряд
        // (нужно реализовать систему сетевых сущностей)
    }
}
*/
/*void GameScene::UpdateNetworkGame(Game* game) {
    float deltaTime = GetFrameTime();
    game->UpdateGameTime(deltaTime);

    NetworkManager* network = game->GetNetworkManager();

    // Обновление физики
    UpdatePhysics(game, deltaTime);

    // Обработка ввода для локального игрока
    HandlePlayerInput(game);

    // Обновление сетевых объектов
    UpdateNetworkPlayers(game);

    // Синхронизация с сетью
    if (localPlayer) {
        network->SendPlayerUpdate(*localPlayer);
    }

    CheckCollisions(game);
    CleanupInactiveEntities();
}*/