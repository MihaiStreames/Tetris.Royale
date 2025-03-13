#ifndef LOBBY_HPP
#define LOBBY_HPP

#include "Common.hpp"
#include "LobbyState.hpp"
#include "ServerRequest.hpp"
#include "ServerResponse.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Lobby
{
    // Lobbies are used to store information about the game session
    // and to manage the players that are connected to it. The lobby
    // is created by the first player that connects to the server.

  public:
    Lobby(const std::string& IPAddr, int port, const std::string& lobbyID,
          GameMode gameMode, int maxPlayers, bool isPublic = true,
          bool debug = false);
    ~Lobby();

    // connectivity management
    [[nodiscard]] StatusCode startLobby();
    [[nodiscard]] StatusCode closeLobby();

    // player management
    [[nodiscard]] StatusCode addPlayer(const std::string& sessionToken,
                                       const std::string& username);
    [[nodiscard]] StatusCode removePlayer(const std::string& sessionToken);
    [[nodiscard]] StatusCode addSpectator(const std::string& sessionToken,
                                          const std::string& username);
    [[nodiscard]] StatusCode removeSpectator(const std::string& sessionToken);

    // info that we might want to use outside of the class
    [[nodiscard]] LobbyState getState();
    [[nodiscard]] int getPort();
    [[nodiscard]] std::string getLobbyID();
    [[nodiscard]] bool isReady();
    void decrementTTL();
    [[nodiscard]] bool isLobbyDead();
    [[nodiscard]] bool isLobbyPublic();

    // utility
    [[nodiscard]] bool isPlayerInLobby(const std::string& sessionToken) const;
    [[nodiscard]] bool
    isSpectatorInLobby(const std::string& sessionToken) const;
    [[nodiscard]] bool isLobbyFull() const;

  private:
    // stuff that is going to be used by the lobby server
    [[nodiscard]] StatusCode initializeSocket();
    [[nodiscard]] StatusCode setSocketOptions();
    [[nodiscard]] StatusCode listen();

    void printMessage(const std::string& message, MessageType msgtype) const;

    // handling requests stuff
    [[nodiscard]] std::string handleRequest(const std::string& requestContent);
    [[nodiscard]] ServerResponse
    handleGetCurrentLobbyRequest(const ServerRequest& request);
    [[nodiscard]] ServerResponse
    handleLeaveLobbyRequest(const ServerRequest& request);
    [[nodiscard]] ServerResponse
    handleReadyRequest(const ServerRequest& request);
    [[nodiscard]] ServerResponse
    handleUnreadyRequest(const ServerRequest& request);

    std::string ip;
    int port;

    std::mutex runningMutex;
    std::mutex listenMutex;
    int lobbySocket;
    struct sockaddr_in lobbyAddr;
    struct sockaddr_in clientAddr;

    std::thread listenThread;

    std::string lobbyID;
    GameMode gameMode;
    int maxPlayers;
    bool isPublic;
    bool debug;
    bool running = false;
    int ttl = LOBBY_TTL;
    std::unordered_map<std::string, bool> readyPlayers;

    // we keep track of the players and spectators in the lobby, associated with
    // their sessionToken
    std::unordered_map<std::string, std::string> players;
    std::unordered_map<std::string, std::string> spectators;
    std::mutex stateMutex;
};

#endif