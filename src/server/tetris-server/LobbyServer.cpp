#include "LobbyServer.hpp"

LobbyServer::LobbyServer(const std::string &IPAddr, const int listenPort,
                         const bool debug)
    : ip(IPAddr), port(listenPort), debug(debug) {
    // this is the constructor for the lobby server, I'll leave it blank for now
    // but we might want to do some stuff here later
}

LobbyServer::~LobbyServer() {
    // "some of this code was written by mihai, I'm not sure if it's necessary"
    // YES I SWEAR COPILOT FUCKING WROTE THIS LMAOOOOOOO BRO WTF 🤣🤣🤣🤣🤣🤣

    // what I was saying is that since we use shared pointers, resources are
    // going to free themselves, so closing will (normally) always be done by a
    // higher entity (such as the Master Server / Tetris Server) I'll leave this
    // here, I know it's bad practice and dead code and blah blah blah but I
    // might use it later

    // std::lock_guard<std::mutex> lock(runningMutex);
    // if (running) { (void) closeLobbyServer(); }
}

StatusCode
LobbyServer::startLobbyServer() {
    // this starts the lobby server, and return the status code, as it should

    printMessage("Lobby Server starting on " + ip + ":" + std::to_string(port),
                 MessageType::INFO);

    // initialize the socket
    if (initializeSocket() != StatusCode::SUCCESS) {
        printMessage("Failed to initialize the socket", MessageType::CRITICAL);
        return StatusCode::ERROR_INITIALIZING_SOCKET;
    }

    // set the running flag to true
    {
        std::lock_guard lock(runningMutex);
        running = true;
    }

    // start the listening thread (unjoined so it's not blocking)
    listenThread = std::thread(&LobbyServer::listen, this);

    return StatusCode::SUCCESS;
}

StatusCode
LobbyServer::closeLobbyServer() {
    // this closes the lobby server, and return the status code
    // since we don't use many join in the code, this better wait for everything
    // to finish before closing the server, so we don't have any dangling
    // threads or sockets

    // first, we set the running flag to false
    {
        std::lock_guard lock(runningMutex);
        running = false;
    }

    // then, we wait for the listen function to finish (if it is listening)
    // and close the socket
    {
        std::lock_guard lock(listenMutex);
        close(serverSocket);
    }

    // we join the listen thread (shoul be done by now since running is false
    // but anyway)
    if (listenThread.joinable()) {
        listenThread.join();
    }

    // we close all the lobbies
    {
        std::lock_guard lock(lobbiesMutex);
        for (auto &[lobbyID, lobby]: lobbyObjects) {
            (void) lobby->closeLobby();
        }

        // and we clear the maps
        lobbies.clear();
        lobbyObjects.clear();
    }

    // and finally, we clear the client sessions
    {
        std::lock_guard lock(clientMutex);
        clientTokens.clear();
    }

    printMessage("Lobby Server closed", MessageType::INFO);
    return StatusCode::SUCCESS;
}

void
LobbyServer::setGameServer(std::shared_ptr<GameServer> gameServer) {
    // this is used to set the game server that is using this lobby server
    this->gameServer = std::move(gameServer);
}

StatusCode
LobbyServer::addClientSession(const std::string &token,
                              const std::string &username) {
    // this method is used to add a client session to the lobby server

    // we check if the maximum number of players is reached
    if (countPlayers() >= MAX_SESSIONS) {
        printMessage("Maximum number of players reached", MessageType::ERROR);
        return StatusCode::ERROR_MAX_PLAYERS_REACHED;
    }

    std::lock_guard lock(clientMutex);
    const auto it = clientTokens.find(token);

    // we check if the session already exists
    if (it != clientTokens.end()) {
        printMessage("Session already exists", MessageType::ERROR);
        return StatusCode::ERROR_SESSION_ALREADY_EXISTS;
    }

    // we also check if some session exists with the same username.
    // if it does, we remove it and return a slightly different success code
    bool found = false;
    // Shadowing - (smart, thx mihai)
    for (auto it_ = clientTokens.begin(); it_ != clientTokens.end();) {
        if (it_->second == username) {
            found = true;
            it_ = clientTokens.erase(it_);
        } else {
            ++it_;
        }
    }

    // finally, we add the session to the lobby server
    clientTokens[token] = username;

    return found ? StatusCode::SUCCESS_REPLACED_SESSION : StatusCode::SUCCESS;
}

StatusCode
LobbyServer::removeClientSession(const std::string &token) {
    // we lock the mutex
    std::lock_guard lock(clientMutex);
    const auto it = clientTokens.find(token);

    // we check if the session exists
    if (it == clientTokens.end()) {
        printMessage("Session not found", MessageType::ERROR);
        return StatusCode::ERROR_SESSION_NOT_FOUND;
    }

    // if it exists, we remove it
    clientTokens.erase(it);
    return StatusCode::SUCCESS;
}

std::string
LobbyServer::getClientSessionUsername(const std::string &token) const {
    // this is used to get the username of a session using the token
    // will throw an error if the session is not found

    if (!isSessionActive(token)) {
        throw std::runtime_error("Session [" + token + "] is not active");
    }

    std::lock_guard lock(clientMutex);
    return clientTokens.at(token);
}

std::string
LobbyServer::getClientSessionToken(const std::string &username) const {
    // this is used to get the token of a session using the username
    // will throw an error if the session is not found

    if (!doesUserHaveSession(username)) {
        throw std::runtime_error("User [" + username +
                                 "] does not have a session");
    }

    std::lock_guard lock(clientMutex);

    std::string foundToken;
    for (const auto &[token, name]: clientTokens) {
        if (name == username) {
            foundToken = token;
            break;
        }
    }
    return foundToken;
}

std::shared_ptr<Lobby>
LobbyServer::getLobby(const std::string &lobbyID) const {
    // this will return a shared pointer to the lobby if it exists, otherwise a
    // nullptr

    // we lock the mutex
    std::lock_guard lock(lobbiesMutex);
    const auto it = lobbyObjects.find(lobbyID);

    // we check if the lobby exists
    if (it == lobbyObjects.end()) {
        printMessage("Lobby [" + lobbyID + "] not found", MessageType::ERROR);
        return nullptr;
    }

    // if it exists, we return it
    return it->second;
}

int
LobbyServer::getLobbyPort(const std::string &lobbyID) const {
    // this is used to get the port of a lobby using its ID
    // will return NO_LOBBY_PORT_FOUND if the lobby is not found (which is -1)

    std::lock_guard lock(lobbiesMutex);
    const auto it = lobbies.find(lobbyID);

    return it != lobbies.end() ? it->second : NO_LOBBY_PORT_FOUND;
}

StatusCode
LobbyServer::closeLobby(const std::string &lobbyID) {
    // close the lobby using its ID
    // will return an error code if the lobby is not found
    // and another error code if the lobby can't be closed

    // we check if the lobby exists
    const auto lobby = getLobby(lobbyID);
    if (lobby == nullptr) {
        printMessage("Lobby [" + lobbyID + "] not found", MessageType::ERROR);
        return StatusCode::ERROR_LOBBY_NOT_FOUND;
    }

    // if it exists, we remove it from the maps
    {
        std::lock_guard lock(lobbiesMutex);
        lobbies.erase(lobbyID);
        lobbyObjects.erase(lobbyID);
    }

    // and we close the lobby
    return lobby->closeLobby();
}

int
LobbyServer::countPlayers() const {
    // count players in the lobby server
    std::lock_guard lock(clientMutex);
    return static_cast<int>(clientTokens.size());
}

int
LobbyServer::countLobbies() const {
    // count lobbies in the lobby server
    std::lock_guard lock(lobbiesMutex);
    return static_cast<int>(lobbies.size());
}

bool
LobbyServer::isRunning() {
    // check if the lobby server is running (some kind of getter, but not used
    // internally)
    std::lock_guard lock(runningMutex);
    return running;
}

std::vector<std::shared_ptr<Lobby> >
LobbyServer::getReadyLobbies() const {
    // this is used to get every lobby that is in a ready state.
    // we get a vector of shared pointers to the lobbies that are ready

    // we lock the mutex
    std::vector<std::shared_ptr<Lobby> > readyLobbies;
    std::lock_guard<std::mutex> lock(lobbiesMutex);

    // we iterate through the lobbies and add the ready ones to the vector
    for (auto &[lobbyID, lobby]: lobbyObjects) {
        if (lobby->isReady()) {
            readyLobbies.push_back(lobby);
        }
    }

    return readyLobbies;
}

std::vector<std::shared_ptr<Lobby> >
LobbyServer::getDeadLobbies() const {
    // this is used to get every lobby that is dead.
    // we get a vector of shared pointers to the lobbies that are empty

    // we lock the mutex
    std::vector<std::shared_ptr<Lobby> > deadLobbies;
    std::lock_guard<std::mutex> lock(lobbiesMutex);

    // we iterate through the lobbies and add the dead ones to the vector
    for (auto &[lobbyID, lobby]: lobbyObjects) {
        if (lobby->isLobbyDead()) {
            deadLobbies.push_back(lobby);
        }
    }

    return deadLobbies;
}

std::vector<std::shared_ptr<Lobby> >
LobbyServer::getPublicLobbies() const {
    // this is used to get every lobby that is public.
    // we get a vector of shared pointers to the lobbies that are public

    // we lock the mutex
    std::vector<std::shared_ptr<Lobby> > publicLobbies;
    std::lock_guard<std::mutex> lock(lobbiesMutex);

    // we iterate through the lobbies and add the public ones to the vector
    for (auto &[lobbyID, lobby]: lobbyObjects) {
        printMessage("Checking lobby [" + lobbyID + "]", MessageType::INFO);
        if (lobby->isLobbyPublic()) {
            publicLobbies.push_back(lobby);
        }
    }

    return publicLobbies;
}

StatusCode
LobbyServer::listen() {
    // this is the listen function, it will listen for incoming requests
    // this runs while running is true, and will stop when running is false

    // create the buffer and the client address length
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[MAX_BUFFER_SIZE] = {};

    // set the lobby server as listening
    std::lock_guard lock(listenMutex);

    while (true) {
        // first, we need to check if the lobby server is still running
        {
            std::lock_guard lock_(runningMutex);
            if (!running) {
                break;
            }
        }

        // if the lobby server is still running, we can receive the request
        const ssize_t recvLen = recvfrom(
            serverSocket, buffer, MAX_BUFFER_SIZE, 0,
            reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
        if (recvLen < 0) {
            continue; // timeout
        }

        // handle the request
        std::string requestContent(buffer, recvLen);
        std::string responseContent = handleRequest(requestContent);

        // send the response
        const ssize_t sentLen = sendto(
            serverSocket, responseContent.c_str(), responseContent.size(), 0,
            reinterpret_cast<struct sockaddr *>(&clientAddr), clientAddrLen);
        if (sentLen < 0) {
            continue; // ignore this, player will timeout and try again
        }
    }

    // bye bye listen thread uwu <3
    return StatusCode::SUCCESS;
}

StatusCode
LobbyServer::initializeSocket() {
    // this initializes the socket for the lobby server
    // (create, set options, bind)

    // create the socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        printMessage("Failed to create the socket", MessageType::CRITICAL);
        return StatusCode::ERROR_CREATING_SOCKET;
    }

    // set the socket options
    if (setSocketOptions() != StatusCode::SUCCESS) {
        printMessage("Failed to set the socket options", MessageType::CRITICAL);
        close(serverSocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // bind the socket
    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr),
             sizeof(serverAddr)) < 0) {
        printMessage("Failed to bind the socket", MessageType::CRITICAL);
        close(serverSocket);
        return StatusCode::ERROR_BINDING_SOCKET;
    }

    return StatusCode::SUCCESS;
}

StatusCode
LobbyServer::setSocketOptions() {
    // some option setting, such as timeout and address / port

    // some timeout options
    timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) < 0) {
        printMessage("Failed to set SO_RCVTIMEO: " +
                     std::string(strerror(errno)),
                     MessageType::CRITICAL);
        close(serverSocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // set the server address / port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));

    return StatusCode::SUCCESS;
}

std::unordered_map<std::string, std::shared_ptr<Lobby> >
LobbyServer::getLobbies() const {
    // this is used to get every lobby in the lobby server
    std::lock_guard lock(lobbiesMutex);
    return lobbyObjects;
}

bool
LobbyServer::isSessionActive(const std::string &token) const {
    // this is used to check if a session is active
    std::lock_guard lock(clientMutex);
    return clientTokens.contains(token);
}

bool
LobbyServer::doesUserHaveSession(const std::string &username) const {
    // this is used to check if a user has a session
    std::lock_guard lock(clientMutex);
    return std::any_of(clientTokens.begin(), clientTokens.end(),
                       [&username](const auto &pair) { return pair.second == username; });
}

void
LobbyServer::printMessage(const std::string &message,
                          const MessageType msgtype) const {
    // this method is used to print messages to the console
    // it will only print if the debug flag is set to true
    // and will print the message with the appropriate type
    // and identifier

    if (!debug) {
        return;
    }

    const std::string lobbyIdentifier = "[Lobby Server] ";
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

    const std::string messageToPrint =
            lobbyIdentifier + "[" + msgtype_str + "] " + message;
    std::cout << messageToPrint << std::endl;
}

std::string
LobbyServer::generateToken(const size_t length) {
    // some random function (absolutely hate working with random
    // numbers in C++ but it's the quickest way to do it ik)

    constexpr char charset[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    // randomizer stuff
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    // actual distribution
    std::string token;
    for (size_t i = 0; i < length; ++i) {
        token += charset[dist(generator)];
    }

    return token;
}

std::string
LobbyServer::generateLobbyID(const size_t length) {
    // same as above, but only with uppercase letters (better for lobby IDs imo)
    constexpr char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string lobbyID;
    for (size_t i = 0; i < length; ++i) {
        lobbyID += charset[dist(generator)];
    }

    return lobbyID;
}

int
LobbyServer::findFreePort() const {
    // this is a bit of a hacky way to do it but it works
    // we just try to bind to a port and if it fails we increment
    // I mean I could set up a proxy server to check for free ports
    // and do some fancy stuff but are you really expecting me to go
    // completly out of my way for a simple project ? idk man maybe

    // this will return the first free port it finds IN ORDER
    // and NO_FILE_DESCRIPTOR (= -1) if it can't create a socket
    // and NO_PORT_AVAILABLE (= -2) if it can't find a free port

    int currentPort = FIRST_LOBBY_PORT + countLobbies();
    // starting port is > 5000 (I think the others are
    // reserved by the OS or something like that, might be 1024 firsts)
    constexpr int maxPort = MAX_PORT;

    while (currentPort <= maxPort) {
        const int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            printMessage("Error creating socket to check port availability",
                         MessageType::CRITICAL);
            return NO_FILE_DESCRIPTOR;
        }

        // set the address / port
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
        addr.sin_port = htons(static_cast<uint16_t>(currentPort));

        // we try to bind to the port : if fail then next port bc not free
        if (bind(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0) {
            close(sockfd);
            return currentPort;
        }

        close(sockfd);
        currentPort++;
    }

    // if we reach this point, we couldn't find a free port
    printMessage("No free port available", MessageType::CRITICAL);
    return NO_PORT_AVAILABLE;
}

void
LobbyServer::startLobby(const std::string &lobbyID) const {
    // find the lobby using its ID
    const auto lobby = getLobby(lobbyID);

    // if lobby is found, then we start it
    if (lobby) {
        (void) lobby->startLobby();
    } else {
        printMessage("Lobby not found", MessageType::ERROR);
    }
}

std::string
LobbyServer::handleRequest(const std::string &requestData) {
    // handle the request and return the response
    // the response will be sent back to the client.

    // First of all, we need to deserialize the request
    // and verify that it went well. If it didn't, we
    // return an error response to the client.

    ServerRequest request;

    try {
        request = ServerRequest::deserialize(requestData);
    } catch (std::runtime_error &e) {
        printMessage(std::string(e.what()), MessageType::ERROR);
        return ServerResponse::ErrorResponse(
                    INVALID_ID, StatusCode::ERROR_DESERIALIZING_REQUEST)
                .serialize();
    }

    printMessage("Handling request [" + getServerMethodString(request.method) +
                 "]",
                 MessageType::INFO);

    // then we handle the request properly according to its method called
    // and return the response to the client

    switch (request.method) {
        case ServerMethods::GET_CLIENT_STATUS:
            return handleGetClientStatusRequest(request).serialize();

        case ServerMethods::START_SESSION:
            return handleStartSessionRequest(request).serialize();

        case ServerMethods::END_SESSION:
            return handleEndSessionRequest(request).serialize();

        case ServerMethods::GET_LOBBY:
            return handleGetLobbyRequest(request).serialize();

        case ServerMethods::GET_PUBLIC_LOBBIES:
            return handleGetPublicLobbiesRequest(request).serialize();

        case ServerMethods::CREATE_LOBBY:
            return handleCreateLobbyRequest(request).serialize();

        case ServerMethods::JOIN_LOBBY:
            return handleJoinLobbyRequest(request).serialize();

        case ServerMethods::SPECTATE_LOBBY:
            return handleSpectateLobbyRequest(request).serialize();

        default:
            printMessage("Request [" + getServerMethodString(request.method) +
                         "] not implemented",
                         MessageType::WARNING);
            return ServerResponse::ErrorResponse(request.id,
                                                 StatusCode::ERROR_UNKNOWN_METHOD)
                    .serialize();
    }
}

ServerResponse
LobbyServer::handleStartSessionRequest(const ServerRequest &request) {
    // handle the start session request
    // return the response to the client

    // First, we generate a token for the client
    // Then, we add the client session to the lobby server
    // Finally, we return the token to the client

    std::string token = generateToken(TOKEN_LENGTH);
    const std::string username = request.params.at("username");

    // we add the client session to the lobby server
    const StatusCode ret = addClientSession(token, username);

    // and we return the response to the client
    switch (ret) {
        case StatusCode::ERROR_SESSION_ALREADY_EXISTS:
            return ServerResponse::ErrorResponse(request.id, ret);

        case StatusCode::SUCCESS:
            return ServerResponse::SuccessResponse(request.id, ret,
                                                   {{"token", token}});

        case StatusCode::SUCCESS_REPLACED_SESSION:
            return ServerResponse::SuccessResponse(request.id, ret,
                                                   {{"token", token}});

        default:
            // debug purpose, should never happen
            throw std::runtime_error(
                "Unexpected StatusCode returned from addClientSession");
    }
}

ServerResponse
LobbyServer::handleEndSessionRequest(const ServerRequest &request) {
    // handle the end session request
    // return the response to the client

    // First, we remove the client session from the lobby server
    // Then, we return the response to the client

    // we remove the client session from the lobby server
    const StatusCode ret = removeClientSession(request.params.at("token"));

    // and we return the response to the client
    switch (ret) {
        case StatusCode::ERROR_SESSION_NOT_FOUND:
            return ServerResponse::ErrorResponse(request.id, ret);

        case StatusCode::SUCCESS:
            return ServerResponse::SuccessResponse(request.id, ret);

        default:
            // debug purpose, should never happen
            throw std::runtime_error(
                "Unexpected StatusCode returned from removeClientSession");
    }
}

ServerResponse
LobbyServer::handleGetLobbyRequest(const ServerRequest &request) const {
    // handle the get lobby request
    // return the response to the client

    // First, we get the lobby ID from the client
    // Then, we get the lobby object from the lobby server
    // Finally, we return the lobby object to the client

    const std::string lobbyID = request.params.at("lobbyID");
    const std::shared_ptr<Lobby> lobby = getLobby(lobbyID);

    // and we return the response to the client (if pointer is not nullptr)
    return lobby
               ? ServerResponse::SuccessResponse(
                   request.id, StatusCode::SUCCESS, lobby->getState())
               : ServerResponse::ErrorResponse(
                   request.id, StatusCode::ERROR_LOBBY_NOT_FOUND);
}

ServerResponse
LobbyServer::handleGetPublicLobbiesRequest(const ServerRequest &request) {
    // handle the get public lobbies request
    // return the response to the client

    // First, we get the public lobbies from the lobby server
    // Then, we return the public lobbies to the client

    std::unordered_map<std::string, std::string> data;
    std::vector<std::shared_ptr<Lobby> > publicLobbies = getPublicLobbies();

    // we add the public lobbies to the data
    int i = 0;
    for (const auto &lobby: publicLobbies) {
        data[std::to_string(i)] = lobby->getState().serialize();
        i++;
    }

    // and we return the response to the client
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS,
                                           data);
}

ServerResponse
LobbyServer::handleCreateLobbyRequest(const ServerRequest &request) {
    // handle the create lobby request
    // return the response to the client

    // First, we generate a lobby ID for the lobby
    // Then, we find a free port for the lobby
    // Then, we create the lobby and add it to the lobby server
    // Finally, we return the lobbyState to the client

    // but first, we need to check if the client has a session
    if (!isSessionActive(request.params.at("token"))) {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_NO_SESSION_TOKEN);
    }

    // we check if the maximum number of lobbies is reached
    if (countLobbies() >= MAX_LOBBIES) {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_MAX_LOBBIES_REACHED);
    }

    // we generate the lobby ID and find a free port
    std::string lobbyID = generateLobbyID(LOBBY_ID_LENGTH);
    int port = findFreePort();

    // if we can't find a free port, we return an error response
    if (port == NO_PORT_AVAILABLE) {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_NO_AVAILABLE_PORT);
    }

    if (port == NO_FILE_DESCRIPTOR) {
        return ServerResponse::ErrorResponse(request.id,
                                             StatusCode::ERROR_CREATING_SOCKET);
    }

    // we create the lobby and add it to the lobby server
    GameMode gameMode =
            static_cast<GameMode>(std::stoi(request.params.at("gameMode")));
    int maxPlayers = std::stoi(request.params.at("maxPlayers"));
    bool publicLobby = request.params.at("visibility") == "public";

    // we want to check if the number of players is fine for the game mode
    // (Classic and Royale should be between MIN_LOBBY_SIZE and MAX_LOBBY_SIZE)
    // (Duel should be DUAL_LOBBY_SIZE)
    // if it's not the case, then return ERROR_INVALID_LOBBY_SIZE

    if (gameMode == GameMode::CLASSIC || gameMode == GameMode::ROYALE) {
        if (maxPlayers < MIN_LOBBY_SIZE || maxPlayers > MAX_LOBBY_SIZE) {
            return ServerResponse::ErrorResponse(
                request.id, StatusCode::ERROR_INVALID_LOBBY_SIZE);
        }
    } else if (gameMode == GameMode::DUEL) {
        if (maxPlayers != DUAL_LOBBY_SIZE) {
            return ServerResponse::ErrorResponse(
                request.id, StatusCode::ERROR_INVALID_LOBBY_SIZE);
        }
    } else if (gameMode == GameMode::ENDLESS) {
        if (maxPlayers != ENDLESS_LOBBY_SIZE) {
            return ServerResponse::ErrorResponse(
                request.id, StatusCode::ERROR_INVALID_LOBBY_SIZE);
        }
    } else {
        throw std::runtime_error(
            "[err] Invalid GameMode in createLobbyRequest");
    }

    const auto lobby = std::make_shared<Lobby>(ip, port, lobbyID, gameMode,
                                               maxPlayers, publicLobby, debug);

    // we lock the mutex
    {
        std::lock_guard lock(lobbiesMutex);
        lobbyObjects[lobbyID] = lobby;
        lobbies[lobbyID] = port;
    }

    // we start the lobby
    startLobby(lobbyID);
    // then we return the response to the client
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS,
                                           lobby->getState());
}

ServerResponse
LobbyServer::handleJoinLobbyRequest(const ServerRequest &request) const {
    // handle the join lobby request
    // return the response to the client

    // First, we get the lobby ID from the client
    // Then, we get the lobby object from the lobby server
    // Then, we add the player to the lobby
    // Finally, we return the response to the client

    // but first, we need to check if the client has a session
    if (!isSessionActive(request.params.at("token"))) {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_NO_SESSION_TOKEN);
    }

    // we get the lobby object from the lobby server
    const std::string lobbyID = request.params.at("lobbyID");
    const std::shared_ptr<Lobby> lobby = getLobby(lobbyID);

    // if the lobby is not found, we return an error response
    if (!lobby) {
        return ServerResponse::ErrorResponse(request.id,
                                             StatusCode::ERROR_LOBBY_NOT_FOUND);
    }

    // we add the player to the lobby, using its token and the username the
    // server knows about
    const std::string username =
            getClientSessionUsername(request.params.at("token"));
    const StatusCode ret =
            lobby->addPlayer(request.params.at("token"), username);

    // and we return the response to the client containing the port to connect
    // to
    switch (ret) {
        case StatusCode::ERROR_CLIENT_ALREADY_IN_LOBBY:
            return ServerResponse::ErrorResponse(request.id, ret);

        case StatusCode::ERROR_LOBBY_FULL:
            return ServerResponse::ErrorResponse(request.id, ret);

        case StatusCode::SUCCESS:
            return ServerResponse::SuccessResponse(
                request.id, ret, {{"port", std::to_string(lobby->getPort())}});

        default:
            // debug purpose, should never happen
            throw std::runtime_error(
                "Unexpected StatusCode returned from addPlayer");
    }
}

ServerResponse
LobbyServer::handleSpectateLobbyRequest(const ServerRequest &request) const {
    // handle the spectate lobby request
    // return the response to the client

    // First, we get the lobby ID from the client
    // Then, we get the lobby object from the lobby server
    // Then, we add the spectator to the lobby
    // Finally, we return the response to the client

    // but first, we need to check if the client has a session
    if (!isSessionActive(request.params.at("token"))) {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_NO_SESSION_TOKEN);
    }

    // we get the lobby object from the lobby server
    const std::string lobbyID = request.params.at("lobbyID");
    const std::shared_ptr<Lobby> lobby = getLobby(lobbyID);

    // if the lobby is not found, we return an error response
    if (!lobby) {
        return ServerResponse::ErrorResponse(request.id,
                                             StatusCode::ERROR_LOBBY_NOT_FOUND);
    }

    // we add the spectator to the lobby, using its token and the username the
    // server knows about
    const std::string username =
            getClientSessionUsername(request.params.at("token"));
    const StatusCode ret =
            lobby->addSpectator(request.params.at("token"), username);

    // and we return the response to the client
    switch (ret) {
        case StatusCode::ERROR_CLIENT_ALREADY_IN_LOBBY:
            return ServerResponse::ErrorResponse(request.id, ret);

        case StatusCode::SUCCESS:
            return ServerResponse::SuccessResponse(
                request.id, ret, {{"port", std::to_string(lobby->getPort())}});

        default:
            // debug purpose, should never happen
            throw std::runtime_error(
                "Unexpected StatusCode returned from addSpectator");
    }
}

ServerResponse
LobbyServer::handleGetClientStatusRequest(const ServerRequest &request) const {
    // handle the get client status request
    // return the response to the client

    // we check if the player is connected to the lobby server. if it is, we
    // try to see if it is in a lobby, or a game, or if it is just connected.
    // if it is not connected, we return offline.

    const std::string username = request.params.at("username");

    // we check if the player is connected to the lobby server
    if (!doesUserHaveSession(username)) {
        return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS,
            ClientStatus::OFFLINE);
    }

    // if the player is connected, we get the token
    const std::string token = getClientSessionToken(username);

    // then we check if the player is in a lobby
    // but first, we need to lock the mutex

    {
        std::lock_guard lock(lobbiesMutex);

        for (const auto &[lobbyID, lobby]: lobbyObjects) {
            if (lobby->isPlayerInLobby(token)) {
                return ServerResponse::SuccessResponse(
                    request.id, StatusCode::SUCCESS, ClientStatus::IN_LOBBY);
            }

            if (lobby->isSpectatorInLobby(token)) {
                return ServerResponse::SuccessResponse(
                    request.id, StatusCode::SUCCESS, ClientStatus::IN_LOBBY);
            }
        }
    }

    // if reached this point, the player is connected and might be in a game
    // we check if the player is in a game

    if (gameServer->isSessionInAnyGame(token)) {
        return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS,
            ClientStatus::IN_GAME);
    }

    // if reached this point, the player is connected but not in a lobby or a
    // game
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS,
        ClientStatus::IN_MENU);
}
