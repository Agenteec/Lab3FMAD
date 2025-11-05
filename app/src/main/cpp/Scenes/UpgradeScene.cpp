#include "UpgradeScene.h"
#include "Game.h"
#include "Entities/Player.h"

UpgradeScene::UpgradeScene()
        : availableResources(0),
          sceneTimer(30.0f), // 30 секунд на выбор улучшений
          maxSceneTime(30.0f) {
}

void UpgradeScene::Update(Game* game) {
    sceneTimer -= GetFrameTime();

    if (sceneTimer <= 0) {
        // Автоматический переход к следующей волне
        game->ChangeState(IN_GAME);
    }
}

void UpgradeScene::Draw(Game* game) {
    ClearBackground((Color){25, 25, 35, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    const char* title = "УЛУЧШЕНИЯ";
    Vector2 titleSize = MeasureTextEx(font, title, 80, 3);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.1f}, 80, 3, (Color){220, 220, 255, 255});

    DrawTextEx(font, TextFormat("Ресурсы: %d", availableResources),
               (Vector2){width * 0.1f, height * 0.2f}, 40, 1, GREEN);

    DrawUpgradeOptions(game);
    DrawTimer(game);
    DrawCommonElements(game);
}

void UpgradeScene::OnEnter() {
    availableResources = 100; // В реальной игре брать из Game
    sceneTimer = maxSceneTime;
}

void UpgradeScene::DrawUpgradeOptions(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    float startX = width * 0.1f;
    float startY = height * 0.3f;
    float optionWidth = width * 0.35f;
    float optionHeight = 120.0f;
    float margin = 20.0f;

    // Улучшение скорости движения
    if (GuiButton((Rectangle){startX, startY, optionWidth, optionHeight}, "СКОРОСТЬ\n+20%\nСтоимость: 30")) {
        ApplyUpgrade(game, "move_speed");
    }

    // Улучшение урона
    if (GuiButton((Rectangle){startX + optionWidth + margin, startY, optionWidth, optionHeight}, "УРОН\n+25%\nСтоимость: 40")) {
        ApplyUpgrade(game, "damage");
    }

    // Улучшение скорости атаки
    if (GuiButton((Rectangle){startX, startY + optionHeight + margin, optionWidth, optionHeight}, "СКОРОСТЬ АТАКИ\n+15%\nСтоимость: 35")) {
        ApplyUpgrade(game, "attack_speed");
    }

    // Улучшение реактора
    if (GuiButton((Rectangle){startX + optionWidth + margin, startY + optionHeight + margin, optionWidth, optionHeight}, "РЕАКТОР\n+Здоровье\nСтоимость: 50")) {
        ApplyUpgrade(game, "reactor");
    }

    // Кнопка пропуска
    if (GuiButton((Rectangle){width/2.f - 150, height - 150.f, 300.f, 80.f}, "ПРОПУСТИТЬ")) {
        game->ChangeState(IN_GAME);
    }
}

void UpgradeScene::DrawTimer(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    float timerPercent = sceneTimer / maxSceneTime;
    const char* timerText = TextFormat("Время: %.1f", sceneTimer);

    Vector2 textSize = MeasureTextEx(font, timerText, 30, 1);
    DrawTextEx(font, timerText, (Vector2){width - textSize.x - 20, 20}, 30, 1,
               timerPercent < 0.3f ? RED : WHITE);

    // Полоска таймера
    DrawRectangle(width * 0.3f, height * 0.9f, width * 0.4f * timerPercent, 10,
                  timerPercent < 0.3f ? RED : GREEN);
}

void UpgradeScene::ApplyUpgrade(Game* game, const std::string& upgradeType) {
    int cost = 0;

    if (upgradeType == "move_speed" && availableResources >= 30) {
        cost = 30;
        // game->GetPlayer()->UpgradeMoveSpeed();
    }
    else if (upgradeType == "damage" && availableResources >= 40) {
        cost = 40;
        // game->GetPlayer()->UpgradeDamage();
    }
    else if (upgradeType == "attack_speed" && availableResources >= 35) {
        cost = 35;
        // game->GetPlayer()->UpgradeAttackSpeed();
    }
    else if (upgradeType == "reactor" && availableResources >= 50) {
        cost = 50;
        // game->GetReactor()->Upgrade();
    }

    if (cost > 0) {
        availableResources -= cost;
        // game->SpendResources(cost);
    }
}