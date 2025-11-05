#include "NetworkManager.h"
#include <iostream>
#include <cstring>
#include "Game.h"

#include "NetworkManager.h"
#include "Projectile.h"
NetworkManager::NetworkManager()
        : clientHost(nullptr),
          serverHost(nullptr),
          serverPeer(nullptr),
          isConnected(false),
          isServer(false),
          localPlayerId(0),
          nextNetworkId(1),
          snapshotInterpolationTime(0.0f) {

    InitializeENet();
    memset(&networkReactor, 0, sizeof(NetworkReactor));
}

NetworkManager::~NetworkManager() {
    Disconnect();
    CleanupENet();
}

void NetworkManager::InitializeENet() {
    if (enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet." << std::endl;
        return;
    }
}

void NetworkManager::CleanupENet() {
    if (serverHost) {
        enet_host_destroy(serverHost);
        serverHost = nullptr;
    }
    if (clientHost) {
        enet_host_destroy(clientHost);
        clientHost = nullptr;
    }
    enet_deinitialize();
}

bool NetworkManager::StartServer(int port) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    serverHost = enet_host_create(&address, 32, 2, 0, 0);
    if (!serverHost) {
        std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
        return false;
    }

    isServer = true;
    isConnected = true;
    std::cout << "Server started on port " << port << std::endl;
    return true;
}

bool NetworkManager::ConnectToServer(const std::string& address, int port) {
    clientHost = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!clientHost) {
        std::cerr << "An error occurred while trying to create an ENet client host." << std::endl;
        return false;
    }

    ENetAddress enetAddress;
    //enet_address_set_hot(&enetAddress, address.c_str());
    enet_address_set_host(&enetAddress, address.c_str());
    enetAddress.port = port;

    serverPeer = enet_host_connect(clientHost, &enetAddress, 2, 0);
    if (!serverPeer) {
        std::cerr << "No available peers for initiating an ENet connection." << std::endl;
        return false;
    }

    isServer = false;
    std::cout << "Connecting to server " << address << ":" << port << std::endl;
    return true;
}

void NetworkManager::Disconnect() {
    if (serverPeer) {
        enet_peer_disconnect(serverPeer, 0);
        serverPeer = nullptr;
    }

    if (clientHost) {
        enet_host_destroy(clientHost);
        clientHost = nullptr;
    }

    if (serverHost) {
        enet_host_destroy(serverHost);
        serverHost = nullptr;
    }

    isConnected = false;
    connectedPlayers.clear();
    networkProjectiles.clear();
    networkEnemies.clear();
}

void NetworkManager::Update() {
    if (!isConnected) return;

    ENetHost* host = isServer ? serverHost : clientHost;
    if (!host) return;

    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                /*std::cout << "A new client connected from "
                          << event.peer->address.host << ":"
                          << event.peer->address.port << std::endl;*/
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                ProcessPacket(event.packet, &event);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected." << std::endl;
                if (isServer) {
                    // Удаляем игрока при отключении
                    for (auto it = connectedPlayers.begin(); it != connectedPlayers.end(); ) {
                        if (event.peer->connectID == it->first) {
                            if (onPlayerLeave) onPlayerLeave(it->first);
                            it = connectedPlayers.erase(it);
                        } else {
                            ++it;
                        }
                    }
                } else {
                    isConnected = false;
                }
                event.peer->data = nullptr;
                break;

            default:
                break;
        }
    }

    // Обновление интерполяции на клиенте
    if (!isServer) {
        snapshotInterpolationTime += 1.0f / 60.0f; // Предполагаем 60 FPS
        UpdatePlayerInterpolation(1.0f / 60.0f);
    }
}

void NetworkManager::ProcessPacket(ENetPacket* packet, ENetEvent* event) {
    if (packet->dataLength < sizeof(PacketType)) return;

    PacketType type = *reinterpret_cast<PacketType*>(packet->data);

    switch (type) {
        case PACKET_PLAYER_JOIN:
            HandlePlayerJoin(packet, event->peer);
            break;
        case PACKET_PLAYER_LEAVE:
            HandlePlayerLeave(packet);
            break;
        case PACKET_PLAYER_UPDATE:
            HandlePlayerUpdate(packet);
            break;
        case PACKET_PLAYER_INPUT:
            HandlePlayerInput(packet);
            break;
        case PACKET_PLAYER_READY:
            HandlePlayerReady(packet);
            break;
        case PACKET_GAME_START:
            HandleGameStart(packet);
            break;
        case PACKET_REACTOR_UPDATE:
            HandleReactorUpdate(packet);
            break;
        case PACKET_ENEMY_SPAWN:
            HandleEnemySpawn(packet);
            break;
        case PACKET_ENEMY_UPDATE:
            HandleEnemyUpdate(packet);
            break;
        case PACKET_PROJECTILE_CREATE:
            HandleProjectileCreate(packet);
            break;
        case PACKET_PROJECTILE_UPDATE:
            HandleProjectileUpdate(packet);
            break;
        case PACKET_CHAT_MESSAGE:
            HandleChatMessage(packet);
            break;
        default:
            std::cout << "Unknown packet type: " << type << std::endl;
            break;
    }
}

// Обработчики пакетов
void NetworkManager::HandlePlayerJoin(ENetPacket* packet, ENetPeer* peer) {
    if (packet->dataLength < sizeof(PacketType) + sizeof(NetworkPlayer)) return;

    NetworkPlayer* netPlayer = reinterpret_cast<NetworkPlayer*>(packet->data + sizeof(PacketType));

    if (isServer) {
        // Сервер назначает ID и рассылает всем
        netPlayer->id = peer->connectID;
        connectedPlayers[netPlayer->id] = *netPlayer;

        // Рассылаем информацию о новом игроке всем
        ENetPacket* broadcastPacket = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkPlayer), ENET_PACKET_FLAG_RELIABLE);
        PacketType packetType = PACKET_PLAYER_JOIN;
        memcpy(broadcastPacket->data, &packetType, sizeof(PacketType));
        memcpy(broadcastPacket->data + sizeof(PacketType), netPlayer, sizeof(NetworkPlayer));
        BroadcastPacket(broadcastPacket, true);

        // Отправляем новому игроку текущее состояние игры
        for (const auto& player : connectedPlayers) {
            if (player.first != netPlayer->id) {
                ENetPacket* playerPacket = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkPlayer), ENET_PACKET_FLAG_RELIABLE);
                PacketType joinPacketType = PACKET_PLAYER_JOIN;
                memcpy(playerPacket->data, &joinPacketType, sizeof(PacketType));
                memcpy(playerPacket->data + sizeof(PacketType), &player.second, sizeof(NetworkPlayer));
                enet_peer_send(peer, 0, playerPacket);
            }
        }
    } else {
        // Клиент добавляет игрока
        connectedPlayers[netPlayer->id] = *netPlayer;
        if (onPlayerJoin) {
            onPlayerJoin(netPlayer->id, std::string(netPlayer->name));
        }
    }
}

void NetworkManager::HandlePlayerLeave(ENetPacket* packet) {
    if (packet->dataLength < sizeof(PacketType) + sizeof(uint32_t)) return;

    uint32_t playerId = *reinterpret_cast<uint32_t*>(packet->data + sizeof(PacketType));
    connectedPlayers.erase(playerId);

    if (onPlayerLeave) {
        onPlayerLeave(playerId);
    }
}

void NetworkManager::HandlePlayerUpdate(ENetPacket* packet) {
    if (packet->dataLength < sizeof(PacketType) + sizeof(NetworkPlayer)) return;

    NetworkPlayer* netPlayer = reinterpret_cast<NetworkPlayer*>(packet->data + sizeof(PacketType));

    if (isServer) {
        // Сервер проверяет и пересылает обновление
        if (connectedPlayers.find(netPlayer->id) != connectedPlayers.end()) {
            connectedPlayers[netPlayer->id] = *netPlayer;

            // Пересылаем всем, кроме отправителя
            ENetPacket* broadcastPacket = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkPlayer), ENET_PACKET_FLAG_UNSEQUENCED);
            PacketType packetType = PACKET_PLAYER_UPDATE;
            memcpy(broadcastPacket->data, &packetType, sizeof(PacketType));
            memcpy(broadcastPacket->data + sizeof(PacketType), netPlayer, sizeof(NetworkPlayer));
            BroadcastPacketExcept(broadcastPacket, nullptr, false); // Ненадежная отправка
        }
    } else {
        // Клиент обновляет состояние игрока
        connectedPlayers[netPlayer->id] = *netPlayer;

        // Добавляем в буфер для интерполяции
        playerSnapshotBuffer[netPlayer->id].push_back(*netPlayer);

        // Ограничиваем размер буфера
        if (playerSnapshotBuffer[netPlayer->id].size() > 10) {
            playerSnapshotBuffer[netPlayer->id].erase(playerSnapshotBuffer[netPlayer->id].begin());
        }
    }
}

// Реализация методов отправки
void NetworkManager::SendPlayerJoin(const std::string& playerName) {
    NetworkPlayer netPlayer;
    memset(&netPlayer, 0, sizeof(NetworkPlayer));
    strncpy(netPlayer.name, playerName.c_str(), sizeof(netPlayer.name) - 1);
    netPlayer.isAlive = true;
    netPlayer.health = 100;
    netPlayer.maxHealth = 100;
    netPlayer.level = 1;

    ENetPacket* packet = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkPlayer), ENET_PACKET_FLAG_RELIABLE);
    PacketType packetType = PACKET_PLAYER_JOIN;
    memcpy(packet->data, &packetType, sizeof(PacketType));
    memcpy(packet->data + sizeof(PacketType), &netPlayer, sizeof(NetworkPlayer));

    if (isServer) {
        BroadcastPacket(packet, true);
    } else {
        enet_peer_send(serverPeer, 0, packet);
    }
}

void NetworkManager::SendPlayerUpdate(const Player& player) {
    NetworkPlayer netPlayer;
    netPlayer.id = localPlayerId;
    strncpy(netPlayer.name, player.GetName().c_str(), sizeof(netPlayer.name) - 1);
    netPlayer.posX = player.GetPosition().x;
    netPlayer.posY = player.GetPosition().y;
    netPlayer.rotation = player.GetRotation();
    netPlayer.velocityX = player.GetVelocity().x;
    netPlayer.velocityY = player.GetVelocity().y;
    netPlayer.health = 100; // Получать из игрока
    netPlayer.maxHealth = 100;
    netPlayer.level = player.GetLevel();
    netPlayer.isAlive = true;

    ENetPacket* packet = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkPlayer), ENET_PACKET_FLAG_UNSEQUENCED);
    PacketType packetType = PACKET_PLAYER_UPDATE;
    memcpy(packet->data, &packetType, sizeof(PacketType));
    memcpy(packet->data + sizeof(PacketType), &netPlayer, sizeof(NetworkPlayer));

    if (isServer) {
        BroadcastPacket(packet, false);
    } else {
        enet_peer_send(serverPeer, 0, packet);
    }
}

void NetworkManager::SendPlayerInput(float inputX, float inputY, bool isFiring) {
    NetworkInput input;
    input.playerId = localPlayerId;
    input.inputX = inputX;
    input.inputY = inputY;
    input.isFiring = isFiring;
    input.sequence = 0; // Можно добавить последовательность для предсказания

    ENetPacket* packet = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkInput), ENET_PACKET_FLAG_UNSEQUENCED);
    PacketType packetType = PACKET_PLAYER_INPUT;
    memcpy(packet->data, &packetType, sizeof(PacketType));
    memcpy(packet->data + sizeof(PacketType), &input, sizeof(NetworkInput));

    enet_peer_send(serverPeer, 0, packet);
}

void NetworkManager::SendPlayerReady(bool ready) {
    bool readyState = ready;

    ENetPacket* packet = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(bool), ENET_PACKET_FLAG_RELIABLE);
    PacketType packetType = PACKET_PLAYER_READY;
    memcpy(packet->data, &packetType, sizeof(PacketType));
    memcpy(packet->data + sizeof(PacketType), &readyState, sizeof(bool));

    enet_peer_send(serverPeer, 0, packet);
}

void NetworkManager::SendReactorUpdate(const Reactor& reactor) {
    NetworkReactor netReactor;
    netReactor.posX = reactor.GetPosition().x;
    netReactor.posY = reactor.GetPosition().y;
    netReactor.health = reactor.GetHealth();
    netReactor.maxHealth = reactor.GetMaxHealth();
    netReactor.upgradeLevel = reactor.GetUpgradeLevel();
    netReactor.isDestroyed = reactor.IsDestroyed();

    networkReactor = netReactor;

    ENetPacket* packet = enet_packet_create(nullptr, sizeof(PacketType) + sizeof(NetworkReactor), ENET_PACKET_FLAG_RELIABLE);
    PacketType packetType = PACKET_REACTOR_UPDATE;
    memcpy(packet->data, &packetType, sizeof(PacketType));
    memcpy(packet->data + sizeof(PacketType), &netReactor, sizeof(NetworkReactor));

    if (isServer) {
        BroadcastPacket(packet, true);
    } else {
        enet_peer_send(serverPeer, 0, packet);
    }
}

// Реализация остальных методов отправки и обработки...
// (SendEnemySpawn, SendProjectileCreate, и т.д.)

void NetworkManager::BroadcastPacket(ENetPacket* packet, bool reliable) {
    if (!serverHost) return;

    if (!reliable) {
        packet->flags = ENET_PACKET_FLAG_UNSEQUENCED;
    }

    enet_host_broadcast(serverHost, 0, packet);
}

void NetworkManager::BroadcastPacketExcept(ENetPacket* packet, ENetPeer* excludePeer, bool reliable) {
    if (!serverHost) return;

    if (!reliable) {
        packet->flags = ENET_PACKET_FLAG_UNSEQUENCED;
    }

    for (size_t i = 0; i < serverHost->peerCount; ++i) {
        if (&serverHost->peers[i] != excludePeer) {
            enet_peer_send(&serverHost->peers[i], 0, packet);
        }
    }

    // ENet автоматически уничтожает пакет после отправки всем пирам
    // Поэтому не вызываем enet_packet_destroy здесь
}

// Интерполяция для сглаживания движения
void NetworkManager::UpdatePlayerInterpolation(float deltaTime) {
    // Упрощенная интерполяция - используем последние два снимка
    for (auto& buffer : playerSnapshotBuffer) {
        if (buffer.second.size() >= 2) {
            NetworkPlayer& current = connectedPlayers[buffer.first];
            const NetworkPlayer& older = buffer.second[0];
            const NetworkPlayer& newer = buffer.second[1];

            // Линейная интерполяция между двумя снимками
            float alpha = snapshotInterpolationTime; // Нужно нормализовать по времени

            current.posX = older.posX + (newer.posX - older.posX) * alpha;
            current.posY = older.posY + (newer.posY - older.posY) * alpha;
            current.rotation = older.rotation + (newer.rotation - older.rotation) * alpha;
        }
    }
}

void NetworkManager::RemovePlayer(uint32_t playerId) {
    connectedPlayers.erase(playerId);
    playerSnapshotBuffer.erase(playerId);
}

void NetworkManager::ClearProjectiles() {
    networkProjectiles.clear();
}

// Реализация остальных обработчиков пакетов...
void NetworkManager::HandlePlayerInput(ENetPacket* packet) {
    // Сервер обрабатывает ввод игрока
}

void NetworkManager::HandlePlayerReady(ENetPacket* packet) {
    // Обработка готовности игрока
}

void NetworkManager::HandleGameStart(ENetPacket* packet) {
    if (onGameStart) {
        onGameStart();
    }
}

void NetworkManager::HandleReactorUpdate(ENetPacket* packet) {
    NetworkReactor* netReactor = reinterpret_cast<NetworkReactor*>(packet->data + sizeof(PacketType));
    networkReactor = *netReactor;
}

void NetworkManager::HandleEnemySpawn(ENetPacket* packet) {
    NetworkEnemy* netEnemy = reinterpret_cast<NetworkEnemy*>(packet->data + sizeof(PacketType));
    networkEnemies[netEnemy->id] = *netEnemy;
}

void NetworkManager::HandleEnemyUpdate(ENetPacket* packet) {
    NetworkEnemy* netEnemy = reinterpret_cast<NetworkEnemy*>(packet->data + sizeof(PacketType));
    networkEnemies[netEnemy->id] = *netEnemy;
}

void NetworkManager::HandleProjectileCreate(ENetPacket* packet) {
    NetworkProjectile* netProjectile = reinterpret_cast<NetworkProjectile*>(packet->data + sizeof(PacketType));
    networkProjectiles[netProjectile->id] = *netProjectile;
}

void NetworkManager::HandleProjectileUpdate(ENetPacket* packet) {
    NetworkProjectile* netProjectile = reinterpret_cast<NetworkProjectile*>(packet->data + sizeof(PacketType));
    networkProjectiles[netProjectile->id] = *netProjectile;
}

void NetworkManager::HandleChatMessage(ENetPacket* packet) {
    // Обработка чат-сообщений
}

void NetworkManager::SendProjectileCreate(const Projectile& projectile) {
    // --- YOUR IMPLEMENTATION GOES HERE ---
    // This is where you will write the code to serialize
    // the projectile data and send it over the network.
    //
    // For example:
    // ENetPacket* packet = CreateProjectilePacket(projectile);
    // enet_peer_send(m_Peer, 0, packet);
}