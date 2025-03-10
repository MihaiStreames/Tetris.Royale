#ifndef TETRIS_SERVER_HPP
#define TETRIS_SERVER_HPP

#include "Common.hpp"
#include "GameServer.hpp"
#include "LobbyServer.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

class TetrisServer
{
  public:
    TetrisServer(const std::string& ip, int listenPort, bool debug = false);

    ~TetrisServer();

    [[nodiscard]] StatusCode startTetrisServer() const;

    [[nodiscard]] StatusCode closeTetrisServer() const;

    [[nodiscard]] StatusCode restartTetrisServer() const;

    [[nodiscard]] int countPlayers() const;

    [[nodiscard]] int countLobbies() const;

    [[nodiscard]] int countGames() const;

    [[nodiscard]] bool isLobbyServerRunning() const;

    [[nodiscard]] bool isGameServerRunning() const;

  private:
    void printMessage(const std::string& message, MessageType msgtype) const;

    std::string ip;
    int lobbyPort;
    bool debug;

    std::shared_ptr<LobbyServer> lobbyServer;
    std::shared_ptr<GameServer> gameServer;
};

#endif
