#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <../enet/include/enet.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "../Entities/Player.h"
#include "../Entities/Enemy.h"
#include "../Entities/Projectile.h"
#include "../Entities/Reactor.h"

// Типы сетевых пакетов
enum PacketType {
    PACKET_PLAYER_JOIN = 1,
    PACKET_PLAYER_LEAVE,
    PACKET_PLAYER_UPDATE,
    PACKET_PLAYER_INPUT,
    PACKET_PLAYER_READY,
    PACKET_GAME_START,
    PACKET_GAME_STATE,
    PACKET_REACTOR_UPDATE,
    PACKET_ENEMY_SPAWN,
    PACKET_ENEMY_UPDATE,
    PACKET_PROJECTILE_CREATE,
    PACKET_PROJECTILE_UPDATE,
    PACKET_CHAT_MESSAGE,
    PACKET_WAVE_START,
    PACKET_WAVE_END,
    PACKET_GAME_OVER
};

// Структуры сетевых данных
#pragma pack(push, 1)
struct NetworkPlayer {
    uint32_t id;
    char name[32];
    float posX, posY;
    float rotation;
    float velocityX, velocityY;
    int health;
    int maxHealth;
    int level;
    bool isReady;
    bool isAlive;
};

struct NetworkInput {
    uint32_t playerId;
    float inputX, inputY;
    bool isFiring;
    uint32_t sequence;
};

struct NetworkProjectile {
    uint32_t id;
    uint32_t ownerId;
    char ownerType[16];
    float posX, posY;
    float velocityX, velocityY;
    float damage;
    float lifetime;
};

struct NetworkEnemy {
    uint32_t id;
    int type;
    float posX, posY;
    float rotation;
    int health;
    int maxHealth;
    bool isAlive;
};

struct NetworkReactor {
    float posX, posY;
    int health;
    int maxHealth;
    int upgradeLevel;
    bool isDestroyed;
};
#pragma pack(pop)

class NetworkManager {
private:
    ENetHost* clientHost;
    ENetHost* serverHost;
    ENetPeer* serverPeer;

    bool isConnected;
    bool isServer;
    uint32_t localPlayerId;
    uint32_t nextNetworkId;

    std::unordered_map<uint32_t, NetworkPlayer> connectedPlayers;
    std::unordered_map<uint32_t, NetworkProjectile> networkProjectiles;
    std::unordered_map<uint32_t, NetworkEnemy> networkEnemies;
    NetworkReactor networkReactor;

    // Буферы для интерполяции
    std::unordered_map<uint32_t, std::vector<NetworkPlayer>> playerSnapshotBuffer;
    float snapshotInterpolationTime;

    // Коллбэки
    std::function<void(uint32_t, const std::string&)> onPlayerJoin;
    std::function<void(uint32_t)> onPlayerLeave;
    std::function<void(uint32_t, bool)> onPlayerReady;
    std::function<void()> onGameStart;

public:
    NetworkManager();
    ~NetworkManager();

    // Серверные методы
    bool StartServer(int port = 12345);
    void StopServer();

    // Клиентские методы
    bool ConnectToServer(const std::string& address, int port = 12345);
    void Disconnect();

    // Основное обновление
    void Update();

    // Отправка данных
    void SendPlayerJoin(const std::string& playerName);
    void SendPlayerUpdate(const Player& player);
    void SendPlayerInput(float inputX, float inputY, bool isFiring);
    void SendPlayerReady(bool ready);
    void SendReactorUpdate(const Reactor& reactor);
    void SendEnemySpawn(const Enemy& enemy);
    void SendEnemyUpdate(const Enemy& enemy);
    void SendProjectileCreate(const Projectile& projectile);
    void SendProjectileUpdate(const Projectile& projectile);
    void SendChatMessage(const std::string& message);
    void SendWaveStart(int waveNumber);
    void SendGameOver(bool victory);

    // Геттеры
    bool IsConnected() const { return isConnected; }
    bool IsServer() const { return isServer; }
    uint32_t GetLocalPlayerId() const { return localPlayerId; }
    std::unordered_map<uint32_t, NetworkPlayer> GetConnectedPlayers() const { return connectedPlayers; }
    std::unordered_map<uint32_t, NetworkProjectile> GetNetworkProjectiles() const { return networkProjectiles; }
    std::unordered_map<uint32_t, NetworkEnemy> GetNetworkEnemies() const { return networkEnemies; }
    NetworkReactor GetNetworkReactor() const { return networkReactor; }

    // Сеттеры для коллбэков
    void SetOnPlayerJoin(std::function<void(uint32_t, const std::string&)> callback) { onPlayerJoin = callback; }
    void SetOnPlayerLeave(std::function<void(uint32_t)> callback) { onPlayerLeave = callback; }
    void SetOnPlayerReady(std::function<void(uint32_t, bool)> callback) { onPlayerReady = callback; }
    void SetOnGameStart(std::function<void()> callback) { onGameStart = callback; }

    // Вспомогательные методы
    uint32_t GenerateNetworkId() { return nextNetworkId++; }
    void RemovePlayer(uint32_t playerId);
    void ClearProjectiles();

private:
    void InitializeENet();
    void CleanupENet();
    void ProcessPacket(ENetPacket* packet, ENetEvent* event);
    void HandlePlayerJoin(ENetPacket* packet, ENetPeer* peer);
    void HandlePlayerLeave(ENetPacket* packet);
    void HandlePlayerUpdate(ENetPacket* packet);
    void HandlePlayerInput(ENetPacket* packet);
    void HandlePlayerReady(ENetPacket* packet);
    void HandleGameStart(ENetPacket* packet);
    void HandleReactorUpdate(ENetPacket* packet);
    void HandleEnemySpawn(ENetPacket* packet);
    void HandleEnemyUpdate(ENetPacket* packet);
    void HandleProjectileCreate(ENetPacket* packet);
    void HandleProjectileUpdate(ENetPacket* packet);
    void HandleChatMessage(ENetPacket* packet);

    // Серверные методы
    void BroadcastPacket(ENetPacket* packet, bool reliable = true);
    void BroadcastPacketExcept(ENetPacket* packet, ENetPeer* excludePeer, bool reliable = true);
    void SendToAllPlayers(const void* data, size_t dataLength, PacketType type, bool reliable = true);

    // Интерполяция
    void UpdatePlayerInterpolation(float deltaTime);
    NetworkPlayer InterpolatePlayerState(uint32_t playerId, float alpha);
};

#endif