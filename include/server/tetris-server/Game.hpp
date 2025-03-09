
#ifndef GAME_HPP
#define GAME_HPP


#include <string>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ServerRequest.hpp"
#include "ServerResponse.hpp"
#include "KeyStroke.hpp"
#include "LobbyState.hpp"
#include "GameState.hpp"
#include "common.hpp"


// at the moment, this is MINIMAL, because we are only doing
// the server logic, but we will implement real games soon


class Game {

public:

    Game(const std::string &ip, const LobbyState &lobbyState, bool debug = false);
    ~Game();

    [[nodiscard]] StatusCode startGame();
    [[nodiscard]] StatusCode closeGame();


private:

    [[nodiscard]] StatusCode initializeSocket();
    [[nodiscard]] StatusCode setSocketOptions();
    [[nodiscard]] StatusCode listen();
    void updateGame();

    [[nodiscard]] std::unordered_map<std::string, std::string> getPlayers();
    [[nodiscard]] std::unordered_map<std::string, std::string> getSpectators();

    void printMessage(const std::string& message, MessageType msgtype) const;

    [[nodiscard]] std::string handleServerRequest(const std::string &requestContent);
    [[nodiscard]] ServerResponse handleKeyStrokeRequest(const ServerRequest &request);
    [[nodiscard]] ServerResponse handleGetGameStateRequest(const ServerRequest &request);

    // called by the handlers
    [[nodiscard]] ServerResponse handleKeyStroke(const KeyStrokePacket& packet, const ServerRequest& request);
    [[nodiscard]] std::string getGameState(const std::string& token);

    std::string ip;
    int port;
    std::string gameID;

    int gameSocket;
    struct sockaddr_in gameAddr;
    struct sockaddr_in clientAddr;

    LobbyState lobbyState;
    bool running = false;
    bool debug;

    // mutexes and threads
    std::mutex gameMutex;
    std::mutex listenMutex;
    std::mutex runningMutex;
    std::mutex updateMutex;

    std::thread gameRoutineThread;
    std::thread updateThread;
    std::thread listenThread;

};


#endif
