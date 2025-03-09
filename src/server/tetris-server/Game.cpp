#include "Game.hpp"


Game::Game(
    const std::string &ip,
    const LobbyState &lobbyState,
    const bool debug
) : ip(ip),
    lobbyState(lobbyState),
    debug(debug)
{
    // this is the constructor for the Game class
    // this will be used to create a new game instance, using lobbyState data to initialize the game

    // TODO ; more funny stuff
    port = lobbyState.port;
    gameID = lobbyState.lobbyID;
}

Game::~Game() {
    // this is the destructor for the Game class
    // this will be used to close the game and free the resources
    // std::lock_guard<std::mutex> lock(runningMutex);
    // if (running) { (void) closeGame(); }
}


StatusCode Game::startGame() {
    // EL DIABLO = MANAUDOU -> CONFIRMED 🤣

    // This method is used to start the game. It should be called in a new thread.
    // It will start the game and open the socket for the players to connect.
    // this will also start the game loop, which will run until the game is closed.

    printMessage("Game starting on " + ip + ":" + std::to_string(port), MessageType::INFO);

    // initialize the socket
    if (initializeSocket() != StatusCode::SUCCESS) {
        printMessage("Error initializing socket", MessageType::ERROR);
        return StatusCode::ERROR_INITIALIZING_SOCKET;
    }

    // set the running flag to true
    {
        std::lock_guard lock(runningMutex);
        running = true;
    }

    // thread to listen for incoming connections
    listenThread = std::thread(&Game::listen, this);
    // thread to update the game
    updateThread = std::thread(&Game::updateGame, this);

    return StatusCode::SUCCESS;
}

StatusCode Game::closeGame() {
    // This method is used to close the game. It should be called when the game is no longer needed.
    // It will close the game and remove all the players from it.

    // IMPORTANT : WAIT FOR THE RETURN OF THIS FUNCTION BEFORE ANYTHING ELSE

    // first, we need to set the running flag to false
    {
        std::lock_guard lock(runningMutex);
        running = false;
    }

    // then, we wait for the listen function to finish (if it is listening)
    // and the update function to finish
    // and close the socket

    {
        std::lock_guard lockListen(listenMutex);
        std::lock_guard lockUpdate(updateMutex);
        close(gameSocket);
        // todo : close some stuff if needed
    }

    // finally, we join the threads
    if (listenThread.joinable()) { listenThread.join(); }
    if (updateThread.joinable()) { updateThread.join(); }

    printMessage("Game closed", MessageType::INFO);
    return StatusCode::SUCCESS;
}


StatusCode Game::initializeSocket() {
    // This method is used to initialize the socket of the game.
    // It will return an error code if there is an error initializing the socket.
    // It will return success if the socket is initialized successfully.

    // create the socket
    gameSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (gameSocket < 0) {
        printMessage("Error creating socket", MessageType::ERROR);
        return StatusCode::ERROR_CREATING_SOCKET;
    }

    // set the socket options
    if (setSocketOptions() != StatusCode::SUCCESS) {
        printMessage("Error setting socket options", MessageType::ERROR);
        close(gameSocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // bind the socket to the address
    if (bind(gameSocket, reinterpret_cast<sockaddr *>(&gameAddr), sizeof(gameAddr)) < 0) {
        printMessage("Error binding socket", MessageType::ERROR);
        close(gameSocket);
        return StatusCode::ERROR_BINDING_SOCKET;
    }

    return StatusCode::SUCCESS;
}

StatusCode Game::setSocketOptions() {
    // This method is used to set the socket options of the game.
    // It will return an error code if there is an error setting the socket options.
    // It will return success if the socket options are set successfully.

    // set the socket options
    constexpr int opt = 1;

    // reuse address and port
    if (setsockopt(gameSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printMessage("Error setting SO_REUSEADDR: " + std::string(strerror(errno)), MessageType::CRITICAL);
        close(gameSocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }
    if (setsockopt(gameSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        printMessage("Error setting SO_REUSEPORT: " + std::string(strerror(errno)), MessageType::CRITICAL);
        close(gameSocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // and timeout options
    timeval timeout;
    timeout.tv_sec = GAME_TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    if (setsockopt(gameSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        printMessage("Error setting SO_RCVTIMEO: " + std::string(strerror(errno)), MessageType::CRITICAL);
        close(gameSocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // set the server address / port
    gameAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &gameAddr.sin_addr);
    gameAddr.sin_port = htons(static_cast<uint16_t>(port));

    return StatusCode::SUCCESS;
}


StatusCode Game::listen() {
    // This method is used to listen for incoming requests and handle them.
    // It will return an error code if there is an error receiving the request or sending the response.
    // It will return success if the request is handled successfully.

    // create the buffer and the client address length
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[MAX_BUFFER_SIZE] = {};

    // set the game as listening
    std::lock_guard lock(listenMutex);

    while (true) {
        // first, we need to check if the game is still running
        {
            // Shadowed the other lock variable
            std::lock_guard lock_(runningMutex);
            if (!running) { break; }
        }

        // if the game is still running, we can receive the request

        const ssize_t recvLen = recvfrom(gameSocket, buffer, MAX_BUFFER_SIZE, 0,
                                         reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
        if (recvLen < 0) {
            continue; // timeout
        }

        // handle the request
        std::string requestContent(buffer, recvLen);
        std::string responseContent = handleServerRequest(requestContent);

        // send the response
        const ssize_t sentLen = sendto(gameSocket, responseContent.c_str(), responseContent.size(), 0,
                                       reinterpret_cast<struct sockaddr *>(&clientAddr), clientAddrLen);
        if (sentLen < 0) {
            continue; // ignore this, player will timeout and try again
        }
    }

    return StatusCode::SUCCESS;
}

void Game::updateGame() {
    // This method is used to update the game state.
    // It will run in a separate thread and will update the game state every frame.

    std::lock_guard lock(updateMutex);

    while (true) {
        // first, we need to check if the game is still running
        {
            // Shadowed the other lock variable
            std::lock_guard lock_(runningMutex);
            if (!running) { break; }
        }

        // if the game is still running, we can update the game state
        // this will be done in a separate function

        // update the game state (we lock the game mutex to update the game state)
        {
            // Shadowed the other lock variable
            std::lock_guard lock_(gameMutex);
            // TODO : update the game state here
            //        most likely we will call handle routine
            //        for every game engine in a for loop, check if
            //        the game is over and maybe some other infos
        }

        // sleep for a while
        std::this_thread::sleep_for(std::chrono::milliseconds(GAME_UPDATE_INTERVAL));
    }
}


std::unordered_map<std::string, std::string> Game::getPlayers() {
    // This method is used to get the players of the game.
    // It will return a map of the players in the game.

    std::lock_guard lock(gameMutex);
    return lobbyState.players;
}

std::unordered_map<std::string, std::string> Game::getSpectators() {
    // This method is used to get the spectators of the game.
    // It will return a map of the spectators in the game.

    std::lock_guard lock(gameMutex);
    return lobbyState.spectators;
}

void Game::printMessage(const std::string &message, const MessageType msgtype) const {
    // This method is used to print a message to the console.
    // It will print the message with the specified message type.
    // this will only print the message if the debug flag is set to true.

    if (!debug) { return; }

    const std::string gameIdentifier = "[GAME-" + gameID + "] ";
    std::string messageType;

    switch (msgtype) {
        case MessageType::INFO:
            messageType = "[INFO] ";
            break;
        case MessageType::WARNING:
            messageType = "[WARNING] ";
            break;
        case MessageType::ERROR:
            messageType = "[ERROR] ";
            break;
        case MessageType::CRITICAL:
            messageType = "[CRITICAL] ";
            break;
        default:
            messageType = "[UNKNOWN | DEBUG] ";
            break;
    }

    const std::string messageToPrint = gameIdentifier + messageType + message;
    std::cout << messageToPrint << std::endl;
}


std::string Game::handleServerRequest(const std::string &requestContent) {
    // handle the request and return the response
    // the response will be sent back to the client.

    // First of all, we need to deserialize the request
    // and verify that it went well. If it didn't, we
    // return an error response to the client.

    ServerRequest request;
    try {
        request = ServerRequest::deserialize(requestContent);
    } catch (std::runtime_error &e) {
        printMessage("Error deserializing request: " + std::string(e.what()), MessageType::ERROR);
        // we use the INVALID ID since we have no way of knowing the ID of the request that failed
        // (not a valid deserializable JSON string)
        return ServerResponse::ErrorResponse(INVALID_ID, StatusCode::ERROR_DESERIALIZING_REQUEST).serialize();
    }

    // Then, we need to handle the request properly according to its method called
    printMessage("Handling request [" + getServerMethodString(request.method) + "]", MessageType::INFO);

    switch (request.method) {
        case ServerMethods::KEY_STROKE:
            return handleKeyStrokeRequest(request).serialize();
        case ServerMethods::GET_GAME_STATE:
            return handleGetGameStateRequest(request).serialize();
        default:
            printMessage("Request [" + getServerMethodString(request.method) + "] not implemented", MessageType::ERROR);
            return ServerResponse::ErrorResponse(request.id, StatusCode::ERROR_NOT_IMPLEMENTED).serialize();
    }
}

ServerResponse Game::handleKeyStrokeRequest(const ServerRequest &request) {
    // this function will handle the key stroke request
    // it will handle the key stroke request and return a response

    // we deserialize the keystroke from the request
    KeyStrokePacket keyStrokePacket;

    try {
        keyStrokePacket = KeyStrokePacket::deserialize(request.params.at("keystroke"));
    } catch (std::runtime_error &e) {
        printMessage("Error deserializing key stroke: " + std::string(e.what()), MessageType::ERROR);
        return ServerResponse::ErrorResponse(request.id, StatusCode::ERROR_DESERIALIZING_REQUEST);
    }

    if (getSpectators().contains(request.params.at("token"))) {
        // spectators are not allowed to send key strokes
        printMessage("Spectator tried to send a key stroke: " + keyStrokePacket.token, MessageType::ERROR);
        return ServerResponse::ErrorResponse(request.id, StatusCode::ERROR_SPECTATOR_CANNOT_INTERACT);
    }

    if (!getPlayers().contains(request.params.at("token"))) {
        // unknown session tried to send a key stroke
        printMessage("Unknown session tried to send a key stroke: " + keyStrokePacket.token, MessageType::ERROR);
        return ServerResponse::SuccessResponse(request.id, StatusCode::ERROR_NOT_SUPPOSED_TO_HAPPEN);
    }
    // known player tried to send a key stroke, we handle it
    return handleKeyStroke(keyStrokePacket, request);
}

ServerResponse Game::handleGetGameStateRequest(const ServerRequest &request) {
    // this function will handle the get game state request
    // it will handle the get game state request and return a response

    const std::string token = request.params.at("token");
    std::string gameStateContent = getGameState(token);
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS, {{"gamestate", gameStateContent}});
}


// TODO : implement this

ServerResponse Game::handleKeyStroke(const KeyStrokePacket &packet, const ServerRequest &request) {
    // this function will handle the key stroke
    // it will handle the key stroke and update the game state

    // we lock the game mutex to update the game state (to wait for the engines to finish their work)
    std::lock_guard lock(gameMutex);

    printMessage("Handling key stroke: " + std::to_string(static_cast<int>(packet.action)) + " from " + packet.token,
                 MessageType::INFO);

    // TODO : handle the key stroke here
    //        ...
    //        ...
    //        ...

    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS);
}

std::string Game::getGameState(const std::string &token) {
    // this function will get the game state
    // it will get the game state and return it

    // we lock the game mutex to get the game state
    std::lock_guard lock(gameMutex);

    // TODO : get the game state here
    //        ...
    //        ...
    //        ...
    (void) token; // just avoid compiler warning

    return SpectatorState::generateEmptyState().serialize();
}
