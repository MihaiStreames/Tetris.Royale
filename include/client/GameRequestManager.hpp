
#ifndef GAME_REQUEST_MANAGER_HPP
#define GAME_REQUEST_MANAGER_HPP

#include "Common.hpp"
#include "KeyStroke.hpp"
#include "ServerRequest.hpp"
#include "ServerResponse.hpp"

#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class GameRequestManager
{
  public:
    GameRequestManager(const std::string& serverIP = MASTER_SERVER_IP,
                       int lobbyServerPort = LOBBY_SERVER_PORT);
    ~GameRequestManager();

    // methods :
    [[nodiscard]] StatusCode connectToServer();
    [[nodiscard]] StatusCode disconnectFromServer();

    // status thing
    [[nodiscard]] ServerResponse getPlayerStatus(const std::string& username);

    // main menu stuff
    [[nodiscard]] ServerResponse startSession(const std::string& username);
    [[nodiscard]] ServerResponse endSession(const std::string& token);
    [[nodiscard]] ServerResponse getPublicLobbiesList();
    [[nodiscard]] ServerResponse createAndJoinLobby(const std::string& token,
                                                    GameMode gameMode,
                                                    int maxPlayers,
                                                    bool isPublic);
    [[nodiscard]] ServerResponse joinLobby(const std::string& token,
                                           const std::string& lobbyID);
    [[nodiscard]] ServerResponse spectateLobby(const std::string& token,
                                               const std::string& lobbyID);

    // lobby stuff
    [[nodiscard]] ServerResponse getCurrentLobbyState(const std::string& token);
    [[nodiscard]] ServerResponse leaveLobby(const std::string& token);
    [[nodiscard]] ServerResponse readyUp(const std::string& token);
    [[nodiscard]] ServerResponse unreadyUp(const std::string& token);

    // game stuff
    [[nodiscard]] ServerResponse
    sendKeyStroke(const std::string& token, const KeyStrokePacket& keyStroke);
    [[nodiscard]] ServerResponse getGameState(const std::string& token);

  private:
    // private here
    StatusCode sendRequest(const ServerRequest& request);
    ServerResponse receiveResponse();

    // socket management
    StatusCode setSocketOptions();
    int getCurrentPort();
    StatusCode changePort(int newPort);
    StatusCode restoreListeningPort();

    // utils
    static int generateRequestID();

    // config
    std::string serverIP;
    int lobbyServerPort;

    // network stuff
    int clientSocket;
    struct sockaddr_in serverAddress;
};

#endif