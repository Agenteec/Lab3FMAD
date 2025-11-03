//
// Created by Admin on 02.11.2025.
//
#include "MainMenuScene.h"
#include "../Game.h"

void MainMenuScene::Update(Game* game) {
}

void MainMenuScene::Draw(Game* game) {
    DrawBackground();
    DrawTitle(game);
    DrawMenuButtons(game);
    DrawCommonElements(game);
}

void MainMenuScene::OnEnter() {
}

void MainMenuScene::DrawBackground() {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    for (int i = 0; i < height; i++) {
        float factor = (float)i / height;
        Color gradientColor = {
                (unsigned char)(45 + factor * 10),
                (unsigned char)(45 + factor * 20),
                (unsigned char)(57 + factor * 30),
                255
        };
        DrawRectangle(0, i, width, 1, gradientColor);
    }

    DrawCircle(width * 0.2f, height * 0.3f, 80, (Color){100, 100, 200, 50});
    DrawCircle(width * 0.8f, height * 0.4f, 60, (Color){200, 100, 100, 50});
    DrawCircle(width * 0.3f, height * 0.7f, 100, (Color){100, 200, 100, 30});
}

void MainMenuScene::DrawTitle(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    const char* title = "КОСМИЧЕСКАЯ БИТВА";
    Vector2 titleSize = MeasureTextEx(font, title, 120, 3);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.15f}, 120, 3, (Color){220, 220, 255, 255});

    const char* subtitle = "Galactic Odyssey";
    Vector2 subtitleSize = MeasureTextEx(font, subtitle, 50, 2);
    DrawTextEx(font, subtitle, (Vector2){(width - subtitleSize.x) / 2, height * 0.15f + titleSize.y + 20}, 50, 2, (Color){180, 180, 220, 255});
}

void MainMenuScene::DrawMenuButtons(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    float buttonWidth = 500;
    float buttonHeight = 100;
    float buttonX = (width - buttonWidth) / 2;
    float startY = height * 0.45f;
    float spacing = 120;

    if (GuiButton((Rectangle){buttonX, startY, buttonWidth, buttonHeight}, "ЗАПУСК ИГРЫ")) {
        game->ChangeState(IN_GAME);
    }

    if (GuiButton((Rectangle){buttonX, startY + spacing, buttonWidth, buttonHeight}, "НАСТРОЙКИ")) {
        game->ChangeState(SETTINGS);
    }

    if (GuiButton((Rectangle){buttonX, startY + spacing * 2, buttonWidth, buttonHeight}, "ОБ ИГРЕ")) {
        game->ChangeState(ABOUT);
    }

    if (GuiButton((Rectangle){buttonX, startY + spacing * 3, buttonWidth, buttonHeight}, "ВЫХОД")) {
        CloseWindow();
    }
}