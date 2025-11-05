#define RAYGUI_IMPLEMENTATION
#include "Game/Game.h"
#include "exception"

int main(void) {
    Game game;

    try {
        game.Run();
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "Game crashed: %s", e.what());
        return -1;
    }
}