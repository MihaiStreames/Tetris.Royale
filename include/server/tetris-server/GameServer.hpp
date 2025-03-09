#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <unordered_map>

#include "LobbyServer.hpp"
#include "Game.hpp"
#include "common.hpp"


class GameServer {

    public:

        GameServer(const std::string& ip, const std::shared_ptr<LobbyServer> &lobbyServer, bool debug = false);
        ~GameServer();

        [[nodiscard]] StatusCode startGameServer();
        [[nodiscard]] StatusCode closeGameServer();

        [[nodiscard]] int countGames();
        [[nodiscard]] bool isRunning();

        


    private:

        void listen();
        void startGame(const std::shared_ptr<Lobby> &lobby);
        void printMessage(const std::string& message, MessageType msgType) const;


        std::string ip;
        std::shared_ptr<LobbyServer> lobbyServer;
        bool debug;
        bool running = false;

        std::vector<std::shared_ptr<Game>> activeGames;
        std::vector<std::thread> gameThreads = {};

        // mutexes and threads
        std::mutex gamesMutex;
        std::mutex runningMutex;
        std::mutex listenMutex;

        std::thread listenThread;


};


#endif