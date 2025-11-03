//
// Created by Admin on 02.11.2025.
//
#include "SettingsScene.h"
#include "../Game.h"
void SettingsScene::Update(Game* game) {
    // Обновление логики настроек
}

void SettingsScene::Draw(Game* game) {
    ClearBackground((Color){30, 30, 40, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);
    GameSettings& settings = game->GetSettings();

    const char* title = "НАСТРОЙКИ";
    Vector2 titleSize = MeasureTextEx(font, title, 100, 3);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.1f}, 100, 3, (Color){220, 220, 255, 255});

    float startY = height * 0.3f;
    float elementWidth = 600;
    float elementHeight = 60;
    float elementX = (width - elementWidth) / 2;
    float spacing = 90;

    DrawTextEx(font, "Язык:", (Vector2){elementX, startY + spacing * 3 - 40}, 40, 1, WHITE);
    if (GuiButton((Rectangle){elementX, startY + spacing * 3, 300, elementHeight}, settings.language == 0 ? "РУССКИЙ" : "ENGLISH")) {
        settings.language = 1 - settings.language;
    }

    if (GuiButton((Rectangle){width/2.f - 150, height - 150.f, 300, 80}, "НАЗАД")) {
        game->ChangeState(MAIN_MENU);
    }

    DrawCommonElements(game);
}