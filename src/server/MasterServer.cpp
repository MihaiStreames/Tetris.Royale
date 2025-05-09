#include "MasterServer.hpp"

MasterServer::MasterServer(const std::string &_ip, const int _lobbyPort,
                           const int _DBPort, const bool _debug)
    : ip(_ip), lobbyPort(_lobbyPort), dbPort(_DBPort), debug(_debug) {
    // constructor for MasterServer
    printMessage("MasterServer created", MessageType::INFO);
}

MasterServer::~MasterServer() {
    // destructor for MasterServer
    // going to close the MasterServer
    (void) closeMasterServer();
}

StatusCode
MasterServer::startMasterServer() {
    // start the MasterServer
    // start the TetrisServer and the DBServer
    // return OK if successful, otherwise return ERROR

    // Start TetrisServer
    tetrisServer = std::make_shared<TetrisServer>(ip, lobbyPort, debug);
    if (tetrisServer->startTetrisServer() != StatusCode::SUCCESS) {
        // I think both display the error message actually so it can be done
        // this way
        printMessage("Failed to start TetrisServer", MessageType::CRITICAL);
        return StatusCode::ERROR;
    }

    // Start DBServer
    dbServer =
            std::make_shared<TetrisDBServer>("0.0.0.0", dbPort, "db-files/main.db");
    if (dbServer->startDBServer() != StatusCode::SUCCESS) {
        // I think both display the error message actually so it can be done
        // this way
        printMessage("Failed to start DBServer", MessageType::CRITICAL);
        return StatusCode::ERROR;
    }

    printMessage("MasterServer started", MessageType::INFO);
    return StatusCode::SUCCESS;
}

StatusCode
MasterServer::closeMasterServer() const {
    // close the MasterServer
    // close the TetrisServer and the DBServer
    // return SUCCESS if successful, otherwise return the error status.

    // Close the TetrisServer
    if (isTetrisServerRunning()) {
        // I think both display the error message actually so it can be done
        // this way
        if (tetrisServer->closeTetrisServer() != StatusCode::SUCCESS) {
            printMessage("Failed to close TetrisServer", MessageType::CRITICAL);
            return StatusCode::ERROR;
        }
    }

    if (isDBServerRunning()) {
        // I think both display the error message actually so it can be done
        // this way
        if (dbServer->closeDBServer() != StatusCode::SUCCESS) {
            printMessage("Failed to close DBServer", MessageType::CRITICAL);
            return StatusCode::ERROR;
        }
    }

    printMessage("MasterServer closed", MessageType::INFO);
    return StatusCode::SUCCESS;
}

StatusCode
MasterServer::restartMasterServer() {
    // restart the MasterServer
    // close the MasterServer and then start it again
    // return SUCCESS if successful, otherwise return the error status.

    StatusCode status = closeMasterServer();
    if (status != StatusCode::SUCCESS) {
        printMessage("Failed to close MasterServer", MessageType::CRITICAL);
        return status;
    }

    status = startMasterServer();
    if (status != StatusCode::SUCCESS) {
        printMessage("Failed to start MasterServer", MessageType::CRITICAL);
        return status;
    }

    printMessage("MasterServer restarted", MessageType::INFO);
    return StatusCode::SUCCESS;
}

int
MasterServer::countPlayers() {
    if (isTetrisServerRunning()) {
        return tetrisServer->countPlayers();
    }

    return 0;
}

int
MasterServer::countLobbies() {
    if (isTetrisServerRunning()) {
        return tetrisServer->countLobbies();
    }

    return 0;
}

int
MasterServer::countGames() {
    if (isTetrisServerRunning()) {
        return tetrisServer->countGames();
    }

    return 0;
}

bool
MasterServer::isTetrisServerRunning() const {
    return tetrisServer && (tetrisServer->isGameServerRunning() &&
                            tetrisServer->isLobbyServerRunning());
}

// DB stuff, completely forgot
bool
MasterServer::isDBServerRunning() const {
    return dbServer && dbServer->isDBServerRunning();
}

void
MasterServer::printMessage(const std::string &message,
                           const MessageType msgtype) const {
    // print a message to the console
    // based on the message type, print the message in a certain color

    if (!debug) {
        return;
    }

    const std::string senderIdentifier = "[MASTER SERVER] ";
    std::string msgIdentifier;

    switch (msgtype) {
        case MessageType::INFO:
            msgIdentifier = "[INFO] ";
            break;
        case MessageType::WARNING:
            msgIdentifier = "[WARNING] ";
            break;
        case MessageType::ERROR:
            msgIdentifier = "[ERROR] ";
            break;
        case MessageType::CRITICAL:
            msgIdentifier = "[CRITICAL] ";
            break;
        default:
            msgIdentifier = "[UNKNOWN] ";
            break;
    }

    std::cout << senderIdentifier << msgIdentifier << message << std::endl;
}

void
MasterServer::handleCommand(const std::string &command) {
    // This function should handle the commands that the user inputs

    if (command == "restart") {
        (void) restartMasterServer();
    } else if (command == "players") {
        std::cout << "Players: " << countPlayers() << std::endl;
    } else if (command == "lobbies") {
        std::cout << "Lobbies: " << countLobbies() << std::endl;
    } else if (command == "games") {
        std::cout << "Games: " << countGames() << std::endl;
    } else {
        std::cout << "Unknown command" << std::endl;
    }
}

int
main(int argc, char *argv[]) {
    // entry point for server stuff
    // create a MasterServer and start it

    // we are not using the arguments for now
    (void) argc;
    (void) argv;

    constexpr bool DEBUG = false;
    MasterServer masterServer(MASTER_SERVER_IP, LOBBY_SERVER_PORT,
                              DB_SERVER_PORT, DEBUG);
    if (masterServer.startMasterServer() != StatusCode::SUCCESS) {
        return EXIT_FAILURE;
    }

    bool running = true;

    // ?? we can do stuff in the runnig loop, but for the moment we will just
    // implement the handleCommand function
    while (running) {
        // we read the input from the user
        std::string input;
        std::getline(std::cin, input);

        // and then handle it (if not exit)
        if (input == "exit") {
            running = false;
        } else {
            masterServer.handleCommand(input);
        }
    }

    // we are done, close the MasterServer
    if (masterServer.closeMasterServer() != StatusCode::SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
