#include "SettingsScene.h"
#include "../Game/Game.h"
#include <iostream>

void SettingsScene::Update(Game* game) {
    // Обновление логики настроек
}

void SettingsScene::Draw(Game* game) {
    ClearBackground((Color){30, 30, 40, 255});

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Font font = GetFont(game);
    GameSettings& settings = game->GetSettings();
    AudioManager* audio = game->GetAudioManager();

    const char* title = "НАСТРОЙКИ";
    Vector2 titleSize = MeasureTextEx(font, title, 100, 3);
    DrawTextEx(font, title, (Vector2){(width - titleSize.x) / 2, height * 0.1f}, 100, 3, (Color){220, 220, 255, 255});

    float startY = height * 0.3f;
    float elementWidth = 600;
    float elementHeight = 60;
    float elementX = (width - elementWidth) / 2;
    float spacing = 90;

    // Громкость музыки
    DrawTextEx(font, "Громкость музыки:", (Vector2){elementX, startY - 40}, 35, 1, WHITE);
    if (GuiSliderBar((Rectangle){elementX, startY, elementWidth, elementHeight},
                     "0.0", "1.0", &settings.musicVolume, 0.0f, 1.0f)) {
        audio->SetMusicVolume(settings.musicVolume);
    }

    // Громкость звуков
    DrawTextEx(font, "Громкость звуков:", (Vector2){elementX, startY + spacing - 40}, 35, 1, WHITE);
    if (GuiSliderBar((Rectangle){elementX, startY + spacing, elementWidth, elementHeight},
                     "0.0", "1.0", &settings.soundVolume, 0.0f, 1.0f)) {
        audio->SetSoundVolume(settings.soundVolume);
    }

    // Полноэкранный режим
    DrawTextEx(font, "Полноэкранный режим:", (Vector2){elementX, startY + spacing * 2 - 40}, 35, 1, WHITE);
    if (GuiCheckBox((Rectangle){elementX, startY + spacing * 2, 40, 40},
                    settings.fullscreen ? "ВКЛ" : "ВЫКЛ", &settings.fullscreen)) {
        if (settings.fullscreen) {
            ToggleFullscreen();
        }
    }

    // Вертикальная синхронизация
    DrawTextEx(font, "Вертикальная синхронизация:", (Vector2){elementX, startY + spacing * 3 - 40}, 35, 1, WHITE);
    if (GuiCheckBox((Rectangle){elementX, startY + spacing * 3, 40, 40},
                    settings.vsync ? "ВКЛ" : "ВЫКЛ", &settings.vsync)) {
        SetWindowState(settings.vsync ? FLAG_VSYNC_HINT : 0);
    }

    // Язык
    DrawTextEx(font, "Язык:", (Vector2){elementX, startY + spacing * 4 - 40}, 35, 1, WHITE);
    if (GuiButton((Rectangle){elementX, startY + spacing * 4, 300, elementHeight},
                  settings.language == 0 ? "РУССКИЙ" : "ENGLISH")) {
        settings.language = 1 - settings.language;
    }

    // Имя игрока
    DrawTextEx(font, "Имя игрока:", (Vector2){elementX, startY + spacing * 5 - 40}, 35, 1, WHITE);
    GuiTextBox((Rectangle){elementX, startY + spacing * 5, 400, elementHeight},
               settings.playerName.data(), 32, true);

    if (GuiButton((Rectangle){width/2.f - 150, height - 150.f, 300, 80}, "СОХРАНИТЬ")) {
        game->GetSaveSystem()->SaveSettings(settings);
        game->ChangeState(MAIN_MENU);
    }

    if (GuiButton((Rectangle){width/2.f - 150, height - 70.f, 300, 60}, "НАЗАД")) {
        game->ChangeState(MAIN_MENU);
    }

    DrawCommonElements(game);
}