#include "LobbyScene.h"
#include "Game.h"

LobbyScene::LobbyScene()
        : isReady(false),
          countdownTimer(5.0f),
          countdownActive(false) {

    // Тестовые данные
    players.push_back({"Player1", true, 1});
    players.push_back({"Player2", false, 2});
    players.push_back({"Player3", true, 3});
}

void LobbyScene::Update(Game* game) {
    if (countdownActive) {
        countdownTimer -= GetFrameTime();
        if (countdownTimer <= 0) {
            game->StartGameSession();
            countdownActive = false;
        }
    }

    // Обновление списка игроков из сети
    // if (game->GetNetwork()->IsConnected()) {
    //     UpdatePlayerListFromNetwork(game);
    // }
}

void LobbyScene::Draw(Game* game) {
    ClearBackground((Color){20, 20, 30, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    const char* title = "ЛОББИ";
    Vector2 titleSize = MeasureTextEx(font, title, 80, 3);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.1f}, 80, 3, (Color){220, 220, 255, 255});

    DrawPlayerList(game);
    DrawReadyButton(game);

    if (countdownActive) {
        DrawCountdown(game);
    }

    DrawCommonElements(game);
}

void LobbyScene::OnEnter() {
    countdownTimer = 5.0f;
    countdownActive = false;
    isReady = false;
}

void LobbyScene::OnExit() {
}

void LobbyScene::DrawPlayerList(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    float startY = height * 0.3f;
    float itemHeight = 60.0f;
    float margin = 20.0f;

    for (size_t i = 0; i < players.size(); i++) {
        const auto& player = players[i];
        float y = startY + i * (itemHeight + margin);

        // Фон игрока
        DrawRectangleRounded(
                Rectangle{width * 0.2f, y, width * 0.6f, itemHeight},
                0.3f, 10, (Color){40, 40, 50, 200}
        );

        // Имя игрока
        DrawTextEx(font, player.name.c_str(),
                   (Vector2){width * 0.25f, y + itemHeight / 2 - 15},
                   30, 1, WHITE);

        // Статус готовности
        const char* status = player.isReady ? "ГОТОВ" : "НЕ ГОТОВ";
        Color statusColor = player.isReady ? GREEN : RED;
        DrawTextEx(font, status,
                   (Vector2){width * 0.6f, y + itemHeight / 2 - 15},
                   25, 1, statusColor);
    }
}

void LobbyScene::DrawReadyButton(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    float buttonWidth = 300.0f;
    float buttonHeight = 80.0f;
    float buttonX = (width - buttonWidth) / 2;
    float buttonY = height * 0.7f;

    const char* buttonText = isReady ? "ОТМЕНИТЬ" : "ГОТОВ";
    Color buttonColor = isReady ? GREEN : BLUE;

    if (GuiButton((Rectangle){buttonX, buttonY, buttonWidth, buttonHeight}, buttonText)) {
        isReady = !isReady;

        // Отправка статуса готовности по сети
        // if (game->GetNetwork()->IsConnected()) {
        //     game->GetNetwork()->SendReadyStatus(isReady);
        // }

        // Проверка, все ли готовы
        bool allReady = true;
        for (const auto& player : players) {
            if (!player.isReady) {
                allReady = false;
                break;
            }
        }

        if (allReady && !countdownActive) {
            StartGameCountdown(game);
        }
    }
}

void LobbyScene::DrawCountdown(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    int countdownInt = static_cast<int>(ceilf(countdownTimer));
    const char* countdownText = TextFormat("Игра начнется через: %d", countdownInt);

    Vector2 textSize = MeasureTextEx(font, countdownText, 40, 1);
    DrawTextEx(font, countdownText,
               (Vector2){(width - textSize.x) / 2, height * 0.85f},
               40, 1, YELLOW);
}

void LobbyScene::StartGameCountdown(Game* game) {
    countdownActive = true;
    countdownTimer = 5.0f;
}
