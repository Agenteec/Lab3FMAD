//
// Created by Admin on 02.11.2025.
//

#include "AboutScene.h"
#include "../Game/Game.h"

void AboutScene::Update(Game* game) {
    // Обновление логики экрана "Об игре"
}

void AboutScene::Draw(Game* game) {
    ClearBackground((Color){30, 30, 40, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    const char* title = "ОБ ИГРЕ";
    Vector2 titleSize = MeasureTextEx(font, title, 100, 3);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.1f}, 100, 3, (Color){220, 220, 255, 255});

    const char* description =
            "КОСМИЧЕСКАЯ БИТВА - это захватывающая аркада,\n"
            "где вы управляете звездолетом в глубинах галактики.\n\n"
            "Собирайте ресурсы, уклоняйтесь от астероидов\n"
            "и сражайтесь с вражескими кораблями!\n\n"
            "Версия: 1.0.0\n"
            "Разработчик: Мария Глазунова";

    Vector2 descSize = MeasureTextEx(font, description, 40, 1);
    DrawTextEx(font, description, (Vector2){(width - descSize.x) / 2, height * 0.3f}, 40, 1, (Color){200, 200, 220, 255});

    if (GuiButton((Rectangle){width/2.f - 150, height - 150.f, 300, 80}, "НАЗАД")) {
        game->ChangeState(MAIN_MENU);
    }

    DrawCommonElements(game);
}
