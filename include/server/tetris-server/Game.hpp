
#ifndef GAME_HPP
#define GAME_HPP

#include "Common.hpp"
#include "GameCreator.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "KeyStroke.hpp"
#include "LobbyState.hpp"
#include "ServerRequest.hpp"
#include "ServerResponse.hpp"
#include "TetrisGame.hpp"

#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Game
{
  public:
    Game(const std::string& ip, const LobbyState& lobbyState,
         bool debug = false);
    ~Game();

    [[nodiscard]] StatusCode startGame();
    [[nodiscard]] StatusCode closeGame();

    [[nodiscard]] bool isSessionInGame(const std::string& token);

  private:
    [[nodiscard]] StatusCode initializeSocket();
    [[nodiscard]] StatusCode setSocketOptions();
    [[nodiscard]] StatusCode initializeGames();
    [[nodiscard]] StatusCode initializeEngine();
    [[nodiscard]] StatusCode listen();

    std::shared_ptr<TetrisGame> getGame(const std::string& token);
    void updateGame();

    [[nodiscard]] std::unordered_map<std::string, std::string> getPlayers();
    [[nodiscard]] std::unordered_map<std::string, std::string> getSpectators();

    void printMessage(const std::string& message, MessageType msgtype) const;

    [[nodiscard]] std::string
    handleServerRequest(const std::string& requestContent);
    [[nodiscard]] ServerResponse
    handleKeyStrokeRequest(const ServerRequest& request);
    [[nodiscard]] ServerResponse handleKeyStroke(const KeyStrokePacket& packet,
                                                 const ServerRequest& request);
    [[nodiscard]] ServerResponse
    handleGetGameStateRequest(const ServerRequest& request);
    [[nodiscard]] std::string getGameState(const std::string& token);

    [[nodiscard]] Action getActionFromKeyStroke(const KeyStrokePacket& packet);

    std::string ip;
    int port;
    std::string gameID;

    int gameSocket;
    struct sockaddr_in gameAddr;
    struct sockaddr_in clientAddr;

    LobbyState lobbyState;
    bool running = false;
    bool debug;

    // model stuff (mvc?)
    std::unordered_map<std::string, std::shared_ptr<TetrisGame>> games;
    std::shared_ptr<GameEngine> engine;
    std::unordered_map<std::string, Action> actionMap;

    // mutexes and threads
    std::mutex gameMutex;
    std::mutex actionMutex;
    std::mutex listenMutex;
    std::mutex runningMutex;
    std::mutex updateMutex;

    std::thread gameRoutineThread;
    std::thread updateThread;
    std::thread listenThread;
};

#endif
