#define RAYGUI_IMPLEMENTATION
#include "Game/Game.h"
#include <exception>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

const char* GetInternalDataPath() {
    return "/data/data/com.example.lab3fmad/files/";
}

void LogErrorToFile(const std::string& message) {
    std::string filePath = std::string(GetInternalDataPath()) + "crash_log.txt";

    std::ofstream logFile(filePath, std::ios_base::app);

    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");

        logFile << "[" << ss.str() << "] " << message << std::endl;
    }
}

int main(void) {
    Game game;

    try {
        game.Run();
    } catch (const std::exception& e) {
        std::stringstream errorMessage;
        errorMessage << "Game crashed: " << e.what();
        TraceLog(LOG_ERROR, "%s", errorMessage.str().c_str());
        LogErrorToFile(errorMessage.str());

        return -1;
    }

    return 0;
}