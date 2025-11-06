cmake_minimum_required(VERSION 4.1.2)

project("main")

set(CHIPMUNK_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CHIPMUNK_BUILD_SHARED OFF CACHE BOOL "" FORCE)

add_subdirectory(Chipmunk2D)

set(ENET_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(enet)
add_library(${CMAKE_PROJECT_NAME} SHARED
        main.cpp
        Game/Game.cpp
        Scenes/Scene.cpp
        Scenes/MainMenuScene.cpp
        Scenes/GameScene.cpp
        Scenes/SettingsScene.cpp
        Scenes/AboutScene.cpp
        Scenes/LobbyScene.cpp
        Scenes/UpgradeScene.cpp
        Physics/PhysicsEngine.cpp
        Entities/Entity.cpp
        Entities/PhysicsEntity.cpp
        Entities/Player.cpp
        Entities/Enemy.cpp
        Entities/Projectile.cpp
        Entities/Reactor.cpp
        Network/NetworkManager.cpp
        Components/CombatComponent.cpp
        ParticleSystem.cpp
        AudioManager.cpp
        SaveSystem.cpp
        Utils/MathUtils.cpp)


target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Game
        ${CMAKE_CURRENT_SOURCE_DIR}/Scenes
        ${CMAKE_CURRENT_SOURCE_DIR}/Physics
        ${CMAKE_CURRENT_SOURCE_DIR}/Entities
        ${CMAKE_CURRENT_SOURCE_DIR}/Network
        ${CMAKE_CURRENT_SOURCE_DIR}/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/enet/include
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
string(REGEX MATCH "android-([0-9]+)" _ ${ANDROID_PLATFORM} FORCE)
set(ANDROID_API_VERSION ${CMAKE_MATCH_1} CACHE STRING "" FORCE)
set(ANDROID_ARCH ${CMAKE_ANDROID_ARCH} CACHE STRING "" FORCE)
set(PLATFORM "Android" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_subdirectory(raylib)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        Scenes
        Game
        Physics
        Entities
        Network
        Components
        Utils
        #chipmunk/include
        enet/include
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        raylib
        enet::enet_static
        chipmunk_static
        android
        log
)

