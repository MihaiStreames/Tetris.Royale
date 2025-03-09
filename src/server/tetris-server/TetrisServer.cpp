#include "TetrisServer.hpp"


TetrisServer::TetrisServer(
    const std::string &ip,
    int listenPort,
    bool debug
) : ip(ip),
    lobbyPort(listenPort),
    debug(debug)
{
    // this is the constructor for the TetrisServer class
    // it will create a new lobby server and start it
    // it will also create a new game server and start it
    // the lobby server will be running on the ip and port given
    // the game server will not be running on any port, so to say, but
    // will handle the lobbies created by the lobby server (on their allocated ports)

    lobbyServer = std::make_shared<LobbyServer>(ip, listenPort, debug);
    gameServer = std::make_shared<GameServer>(ip, lobbyServer, debug);
}


TetrisServer::~TetrisServer() {
    // this is the destructor for the TetrisServer class
    // it will stop the lobby server and the game server
    // it will also delete the lobby server and the game server (auto)

    // if (lobbyServer) { (void) closeTetrisServer(); }
}


StatusCode TetrisServer::startTetrisServer() const {
    // this method will start the lobby server and the game server
    // this does NOT start the servers in separate threads, rather
    // it starts the servers and waits for them to finish starting
    // the close method will do the same thing, but for stopping the servers

    printMessage("Starting Tetris Server on " + ip, MessageType::INFO);

    const StatusCode gssc = gameServer->startGameServer();
    if (gssc != StatusCode::SUCCESS) {
        printMessage("Failed to start Game Server", MessageType::ERROR);
        return gssc;
    }

    const StatusCode lbsc = lobbyServer->startLobbyServer();
    if (lbsc != StatusCode::SUCCESS) {
        printMessage("Failed to start Lobby Server", MessageType::ERROR);
        return lbsc;
    }

    return StatusCode::SUCCESS;
}

StatusCode TetrisServer::closeTetrisServer() const {
    // this method will stop the lobby server and the game server
    // it will wait for both servers to finish and then return the status code SUCCESS

    printMessage("Closing Tetris Server", MessageType::INFO);

    const StatusCode gssc = gameServer->closeGameServer();
    if (gssc != StatusCode::SUCCESS) {
        printMessage("Failed to close Game Server", MessageType::ERROR);
        return gssc;
    }

    const StatusCode lbsc = lobbyServer->closeLobbyServer();
    if (lbsc != StatusCode::SUCCESS) {
        printMessage("Failed to close Lobby Server", MessageType::ERROR);
        return lbsc;
    }

    printMessage("Tetris Server closed", MessageType::INFO);
    return StatusCode::SUCCESS;
}


StatusCode TetrisServer::restartTetrisServer() const {
    // this method will restart the lobby server and the game server
    // it will close the servers and then start them again, effectively restarting them
    // and forgetting all the previous data

    const StatusCode csc = closeTetrisServer();
    if (csc != StatusCode::SUCCESS) {
        printMessage("Failed to close Tetris Server", MessageType::ERROR);
        return csc;
    }

    const StatusCode ssc = startTetrisServer();
    if (ssc != StatusCode::SUCCESS) {
        printMessage("Failed to start Tetris Server", MessageType::ERROR);
        return ssc;
    }

    return StatusCode::SUCCESS;
}


int TetrisServer::countPlayers() const {
    // this method will return the number of players in the lobby server
    return lobbyServer->countPlayers();
}

int TetrisServer::countLobbies() const {
    // this method will return the number of lobbies in the lobby server
    return lobbyServer->countLobbies();
}

int TetrisServer::countGames() const {
    // this method will return the number of games in the game server
    return gameServer->countGames();
}

bool TetrisServer::isLobbyServerRunning() const {
    // this method will return true if the lobby server is running
    return lobbyServer->isRunning();
}

bool TetrisServer::isGameServerRunning() const {
    // this method will return true if the game server is running
    return gameServer->isRunning();
}


void TetrisServer::printMessage(const std::string &message, const MessageType msgtype) const {
    // This method is used to print a message to the console.
    // It will print the message with the specified message type
    // only if the debug flag is set to true.

    if (!debug) { return; }

    const std::string tetrisServerIdentifier = "[Tetris Server] ";
    std::string msgtype_str;

    switch (msgtype) {
        case MessageType::INFO:
            msgtype_str = "INFO";
            break;

        case MessageType::WARNING:
            msgtype_str = "WARNING";
            break;

        case MessageType::ERROR:
            msgtype_str = "ERROR";
            break;

        case MessageType::CRITICAL:
            msgtype_str = "CRITICAL";
            break;

        default:
            msgtype_str = "UNKNOWN | DEBUG";
            break;
    }

    const std::string messageToPrint = tetrisServerIdentifier + "[" + msgtype_str + "] " + message;
    std::cout << messageToPrint << std::endl;
}
