#include "MainMenuScene.h"
#include "../Game/Game.h"

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

    // Градиентный фон
    for (int i = 0; i < height; i++) {
        float factor = (float)i / height;
        Color gradientColor = {
                (unsigned char)(25 + factor * 20),
                (unsigned char)(25 + factor * 20),
                (unsigned char)(35 + factor * 22),
                255
        };
        DrawRectangle(0, i, width, 1, gradientColor);
    }

    // Анимированные планеты и звезды
    float time = GetTime();

    // Планета 1
    DrawCircle(width * 0.2f, height * 0.3f + sinf(time * 0.5f) * 10, 60, (Color){100, 100, 200, 80});
    DrawCircle(width * 0.2f, height * 0.3f + sinf(time * 0.5f) * 10, 40, (Color){120, 120, 220, 100});

    // Планета 2
    DrawCircle(width * 0.8f, height * 0.4f + cosf(time * 0.3f) * 8, 50, (Color){200, 100, 100, 70});
    DrawCircle(width * 0.8f, height * 0.4f + cosf(time * 0.3f) * 8, 30, (Color){220, 120, 120, 90});

    // Мерцающие звезды
    for (int i = 0; i < 50; i++) {
        int x = (i * 137) % width;
        int y = (i * 237) % height;
        float brightness = 0.5f + sinf(time * 2.0f + i) * 0.5f;
        DrawCircle(x, y, 1 + (i % 3), Fade(WHITE, brightness * 0.8f));
    }
}

void MainMenuScene::DrawTitle(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);

    const char* title = "VOID ASSAULT";
    Vector2 titleSize = MeasureTextEx(font, title, 120, 3);

    // Анимированное свечение заголовка
    float glow = sinf(GetTime() * 2.0f) * 0.3f + 0.7f;
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.15f}, 120, 3,
               (Color){220, 220, 255, (unsigned char)(255 * glow)});

    const char* subtitle = "Защити реактор от ИИ";
    Vector2 subtitleSize = MeasureTextEx(font, subtitle, 50, 2);
    DrawTextEx(font, subtitle, (Vector2){(width - subtitleSize.x) / 2, height * 0.15f + titleSize.y + 20},
               50, 2, (Color){180, 180, 220, 255});

    // Версия игры
    const char* version = "Версия 1.0.0";
    DrawTextEx(font, version, (Vector2){width - 150.f, height - 30.f}, 20, 1, GRAY);
}

void MainMenuScene::DrawMenuButtons(Game* game) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    float buttonWidth = 500;
    float buttonHeight = 100;
    float buttonX = (width - buttonWidth) / 2;
    float startY = height * 0.45f;
    float spacing = 120;

    // Одиночная игра
    if (GuiButton((Rectangle){buttonX, startY, buttonWidth, buttonHeight}, "ОДИНОЧНАЯ ИГРА")) {
        game->StartGameSession();
    }

    // Сетевая игра
    if (GuiButton((Rectangle){buttonX, startY + spacing, buttonWidth, buttonHeight}, "СЕТЕВАЯ ИГРА")) {
        game->ChangeState(LOBBY);
    }

    // Настройки
    if (GuiButton((Rectangle){buttonX, startY + spacing * 2, buttonWidth, buttonHeight}, "НАСТРОЙКИ")) {
        game->ChangeState(SETTINGS);
    }

    // Об игре
    if (GuiButton((Rectangle){buttonX, startY + spacing * 3, buttonWidth, buttonHeight}, "ОБ ИГРЕ")) {
        game->ChangeState(ABOUT);
    }

    // Выход
    if (GuiButton((Rectangle){buttonX, startY + spacing * 4, buttonWidth, buttonHeight}, "ВЫХОД")) {
        CloseWindow();
    }

    // Статистика текущего профиля
    SaveSystem* saveSystem = game->GetSaveSystem();
    auto stats = saveSystem->GetPlayerStats(saveSystem->GetCurrentProfile());

    if (stats.highestWave > 0) {
        const char* statsText = TextFormat("Рекорд: Волна %d | Счет: %d", stats.highestWave, stats.totalScore);
        DrawTextEx(GetFont(game), statsText, (Vector2){20, height - 60.f}, 20, 1, GRAY);
    }
}