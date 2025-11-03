cmake_minimum_required(VERSION 4.1.2)

project("main")

add_library(${CMAKE_PROJECT_NAME} SHARED
        main.cpp
        Game.cpp
        Scenes/Scene.cpp
        Scenes/MainMenuScene.cpp
        Scenes/GameScene.cpp
        Scenes/SettingsScene.cpp
        Scenes/AboutScene.cpp)
target_include_directories(main PRIVATE
        Scenes
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
string(REGEX MATCH "android-([0-9]+)" _ ${ANDROID_PLATFORM} FORCE)
set(ANDROID_API_VERSION ${CMAKE_MATCH_1} CACHE STRING "" FORCE)
set(ANDROID_ARCH ${CMAKE_ANDROID_ARCH} CACHE STRING "" FORCE)
set(PLATFORM "Android" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_subdirectory(raylib)
target_link_libraries(${CMAKE_PROJECT_NAME}
        raylib
        android
        log)