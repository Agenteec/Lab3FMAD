#include "Game.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/GameScene.h"
#include "Scenes/SettingsScene.h"
#include "Scenes/AboutScene.h"
#include "Scenes/Scene.h"

Game::Game() : currentState(MAIN_MENU), resourcesLoaded(false) {

    InitWindow(0, 0, "");
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();
    SetWindowSize(width, height);
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTargetFPS(60);


    LoadResources();
    ApplyUIStyle();
    ChangeState(MAIN_MENU);
}

Game::~Game() {
    UnloadResources();
    CloseWindow();
}

void Game::LoadResources() {
    int codepointCount = 0;
    int* completeCodepoints = GetCompleteCodepoints(&codepointCount);

    gameFont = LoadFontEx("fonts/static/Roboto-Regular.ttf", 70, completeCodepoints, codepointCount);

    free(completeCodepoints);

    GuiSetFont(gameFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 58);

    resourcesLoaded = true;
}

void Game::UnloadResources() {
    if (resourcesLoaded && gameFont.texture.id != 0 && gameFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(gameFont);
    }
    resourcesLoaded = false;
}

void Game::ApplyUIStyle() {
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x2d2d39ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x363645ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x404052ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x565670ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x6a6a8fff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x4a4a62ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xffffffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xffffffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xffffffff);

    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, TEXT_PADDING, 20);
    GuiSetStyle(SLIDER, BORDER_WIDTH, 2);
    GuiSetStyle(CHECKBOX, BORDER_WIDTH, 2);
}

int* Game::GetCompleteCodepoints(int* count) {
    static int completeChars[] = {
            0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029,
            0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033,
            0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D,
            0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
            0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051,
            0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B,
            0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065,
            0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
            0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079,
            0x007A, 0x007B, 0x007C, 0x007D, 0x007E,
            0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419,
            0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420, 0x0421, 0x0422, 0x0423,
            0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D,
            0x042E, 0x042F, 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
            0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 0x0440, 0x0441,
            0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B,
            0x044C, 0x044D, 0x044E, 0x044F, 0x0401, 0x0451
    };

    *count = sizeof(completeChars) / sizeof(completeChars[0]);
    int* result = (int*)malloc(*count * sizeof(int));
    memcpy(result, completeChars, *count * sizeof(int));
    return result;
}

void Game::Run() {
    while (!WindowShouldClose()) {
        currentScene->Update(this);

        BeginDrawing();
        ClearBackground(BLACK);
        currentScene->Draw(this);
        EndDrawing();
    }
}

void Game::ChangeState(GameState newState) {
    currentState = newState;

    switch (newState) {
        case MAIN_MENU:
            currentScene = std::make_unique<MainMenuScene>();
            break;
        case IN_GAME:
            currentScene = std::make_unique<GameScene>();
            break;
        case SETTINGS:
            currentScene = std::make_unique<SettingsScene>();
            break;
        case ABOUT:
            currentScene = std::make_unique<AboutScene>();
            break;
    }

    if (currentScene) {
        currentScene->OnEnter();
    }
}

void Game::DrawBottomRightPanel() {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    float panelSize = 80;
    float margin = 20;
    float iconSize = 60;

    DrawRectangleRounded((Rectangle){width - panelSize * 2 - margin * 2, height - panelSize - margin,
                                     panelSize * 2, panelSize}, 0.3f, 10, (Color){40, 40, 50, 200});

    const char* langText = settings.language == 0 ? "RU" : "EN";
    if (GuiButton((Rectangle){width - panelSize * 2 - margin, height - panelSize - margin + 10,
                              iconSize, iconSize}, langText)) {
        settings.language = 1 - settings.language;
    }

    if (GuiButton((Rectangle){width - panelSize - margin + 10, height - panelSize - margin + 10,
                              iconSize, iconSize}, "i")) {
        // Действие для кнопки информации
    }
}