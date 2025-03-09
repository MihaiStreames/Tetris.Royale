#ifndef MASTER_SERVER_HPP
#define MASTER_SERVER_HPP

#include <string>
#include <iostream>
#include <memory>

#include "TetrisServer.hpp"
#include "DBServer.hpp"
#include "common.hpp"


class MasterServer {

public:
    // Compiler optimizations? (Another CLion suggestion)
    explicit MasterServer(
        const std::string& _ip = MASTER_SERVER_IP,
        int _lobbyPort = LOBBY_SERVER_PORT,
        int _DBPort = DB_SERVER_PORT,
        bool _debug = false
    );

    ~MasterServer();

    [[nodiscard]] StatusCode startMasterServer();
    [[nodiscard]] StatusCode closeMasterServer() const;
    [[nodiscard]] StatusCode restartMasterServer();


    [[nodiscard]] int countPlayers();
    [[nodiscard]] int countLobbies();
    [[nodiscard]] int countGames();
    [[nodiscard]] bool isTetrisServerRunning() const;
    [[nodiscard]] bool isDBServerRunning() const;

    void handleCommand(const std::string& command);


private:

    void printMessage(const std::string& message, MessageType msgtype) const;

    std::string ip;
    int lobbyPort;
    int dbPort;
    bool debug;

    std::shared_ptr<TetrisServer> tetrisServer;
    std::shared_ptr<TetrisDBServer> dbServer;

};


// entry point for server stuff
int main(int argc, char* argv[]);


#endif