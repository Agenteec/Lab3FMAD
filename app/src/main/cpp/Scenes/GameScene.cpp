//
// Created by Admin on 02.11.2025.
//

#include "GameScene.h"
#include "../Game.h"

void GameScene::Update(Game* game) {
    // Обновление игровой логики
}

void GameScene::Draw(Game* game) {
    DrawBackground();
    DrawGameUI(game);
    DrawCommonElements(game);
}

void GameScene::DrawBackground() {
    ClearBackground((Color){10, 10, 20, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    for (int i = 0; i < 100; i++) {
        int x = (i * 137) % width;
        int y = (i * 237) % height;
        int size = (i % 3) + 1;
        DrawCircle(x, y, size, WHITE);
    }
}

void GameScene::DrawGameUI(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    const char* title = "ИГРА В ПРОЦЕССЕ";
    Vector2 titleSize = MeasureTextEx(font, title, 80, 2);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.1f}, 80, 2, (Color){220, 220, 255, 255});

    DrawTextEx(font, TextFormat("Счет: %d", game->GetCounter()), (Vector2){width/2.f - 100, height * 0.3f}, 50, 1, GREEN);

    if (GuiButton((Rectangle){width/2.f - 150, height * 0.5f, 300, 100}, "СОБРАТЬ РЕСУРС")) {
        game->IncrementCounter();
    }

    if (GuiButton((Rectangle){width/2.f - 150, height - 150.f, 300, 80}, "В ГЛАВНОЕ МЕНЮ")) {
        game->ChangeState(MAIN_MENU);
    }
}
