#include "Lobby.hpp"

Lobby::Lobby(const std::string& IPAddr, const int port,
             const std::string& lobbyID, const GameMode gameMode,
             const int maxPlayers, const bool isPublic, const bool debug)
    : ip(IPAddr), port(port), lobbyID(lobbyID), gameMode(gameMode),
      maxPlayers(maxPlayers), isPublic(isPublic), debug(debug)
{
    // this is the constructor for the lobby, I'll leave it blank for now
    // but we might want to do some stuff here later
}

Lobby::~Lobby()
{
    // close the socket (does that even work if the socket is not open?)
    // std::lock_guard<std::mutex> lock(runningMutex);
    // if (running) { (void) closeLobby(); }
}

StatusCode
Lobby::startLobby()
{
    // This method is used to start the lobby. It should be called after the
    // lobby is created. It will start the lobby waiting session and will wait
    // for the players to connect.

    // This does NOT add the creator of the lobby to the lobby,
    // rather it starts the lobby and waits for the creator to join (which will
    // be done automatically via the clientSession class)

    printMessage("Lobby starting on " + ip + ":" + std::to_string(port),
                 MessageType::INFO);

    // initialize the socket
    if (initializeSocket() != StatusCode::SUCCESS)
    {
        printMessage("Error initializing socket", MessageType::ERROR);
        return StatusCode::ERROR_INITIALIZING_SOCKET;
    }

    // set the running flag to true
    {
        std::lock_guard lock(runningMutex);
        running = true;
    }

    // start the lobby listening session
    listenThread = std::thread(&Lobby::listen, this);

    // listen for incoming connections and handle them
    return StatusCode::SUCCESS;
}

StatusCode
Lobby::closeLobby()
{
    // This method is used to close the lobby. It should be called when the
    // lobby is no longer needed. It will close the lobby and remove all the
    // players from it.

    // first, we need to set the running flag to false
    {
        std::lock_guard lock(runningMutex);
        running = false;
    }

    // then, we wait for the listen function to finish (if it is listening)
    // and close the socket

    {
        std::lock_guard lock(listenMutex);
        close(lobbySocket);
    }

    // finally, we join the thread if it is joinable
    if (listenThread.joinable())
    {
        listenThread.join();
    }

    printMessage("Lobby closed", MessageType::INFO);
    return StatusCode::SUCCESS;
}

StatusCode
Lobby::addPlayer(const std::string& sessionToken, const std::string& username)
{
    // This method is used to add a player to the lobby.
    // It will return an error code if the player is already in the lobby, or if
    // the lobby is full. It will return success if the player is added to the
    // lobby.

    std::lock_guard lock(stateMutex);

    // check if the player is already in the lobby
    if (isPlayerInLobby(sessionToken) || isSpectatorInLobby(sessionToken))
    {
        printMessage("Player " + sessionToken + " is already in the lobby",
                     MessageType::ERROR);
        return StatusCode::ERROR_CLIENT_ALREADY_IN_LOBBY;
    }

    // check if the lobby is full
    if (isLobbyFull())
    {
        printMessage("Lobby is full", MessageType::ERROR);
        return StatusCode::ERROR_LOBBY_FULL;
    }

    // if we get here, we can add the player to the lobby
    players[sessionToken] = username;
    readyPlayers[sessionToken] = false;

    printMessage("Player " + sessionToken + " added to the lobby",
                 MessageType::INFO);
    return StatusCode::SUCCESS;
}

StatusCode
Lobby::removePlayer(const std::string& sessionToken)
{
    // This method is used to remove a player from the lobby.
    // It will return an error code if the player is not in the lobby.
    // It will return success if the player is removed from the lobby.

    std::lock_guard lock(stateMutex);

    // check if the player is in the lobby
    if (!isPlayerInLobby(sessionToken))
    {
        // printMessage("Player " + sessionToken + " is not in the lobby",
        // MessageType::ERROR);
        return StatusCode::ERROR_CLIENT_NOT_IN_LOBBY;
    }

    // remove the player from the lobby
    players.erase(sessionToken);
    readyPlayers.erase(sessionToken);

    printMessage("Player " + sessionToken + " removed from the lobby",
                 MessageType::INFO);
    return StatusCode::SUCCESS;
}

StatusCode
Lobby::addSpectator(const std::string& sessionToken,
                    const std::string& username)
{
    // This method is used to add a spectator to the lobby.
    // It will return an error code if the spectator is already in the lobby.
    // It will return success if the spectator is added to the lobby.

    std::lock_guard lock(stateMutex);

    // check if the spectator is already in the lobby
    if (isPlayerInLobby(sessionToken) || isSpectatorInLobby(sessionToken))
    {
        printMessage("Spectator " + sessionToken + " is already in the lobby",
                     MessageType::ERROR);
        return StatusCode::ERROR_CLIENT_ALREADY_IN_LOBBY;
    }

    // if we get here, we can add the spectator to the lobby
    spectators[sessionToken] = username;

    printMessage("Spectator " + sessionToken + " added to the lobby",
                 MessageType::INFO);
    return StatusCode::SUCCESS;
}

StatusCode
Lobby::removeSpectator(const std::string& sessionToken)
{
    // This method is used to remove a spectator from the lobby.
    // It will return an error code if the spectator is not in the lobby.
    // It will return success if the spectator is removed from the lobby.

    std::lock_guard lock(stateMutex);

    // check if the spectator is in the lobby
    if (!isSpectatorInLobby(sessionToken))
    {
        // printMessage("Spectator " + sessionToken + " is not in the lobby",
        // MessageType::ERROR);
        return StatusCode::ERROR_CLIENT_NOT_IN_LOBBY;
    }

    // remove the spectator from the lobby
    spectators.erase(sessionToken);

    printMessage("Spectator " + sessionToken + " removed from the lobby",
                 MessageType::INFO);
    return StatusCode::SUCCESS;
}

LobbyState
Lobby::getState()
{
    // This method is used to get the state of the lobby.
    // It will return the state of the lobby.

    std::lock_guard lock(stateMutex);

    LobbyState state;
    state.lobbyID = lobbyID;
    state.port = port;
    state.gameMode = gameMode;
    state.maxPlayers = maxPlayers;
    state.players = players;
    state.spectators = spectators;
    state.readyPlayers = readyPlayers;

    return state;
}

int
Lobby::getPort()
{
    // This method is used to get the port of the lobby.
    // It will return the port of the lobby.

    std::lock_guard lock(stateMutex);
    return port;
}

std::string
Lobby::getLobbyID()
{
    // This method is used to get the ID of the lobby.
    // It will return the ID of the lobby.

    std::lock_guard lock(stateMutex);
    return lobbyID;
}

bool
Lobby::isReady()
{
    // this method is used to get the ready status of the lobby.
    // it will return true if all the players are ready, false otherwise.

    std::lock_guard lock(stateMutex);

    // first, we check if the number of players in the lobby is enough to start
    // the game
    if (static_cast<int>(players.size()) < MIN_LOBBY_SIZE)
    {
        return false;
    }

    // then if we have enough players, we check if all of them are ready
    return std::ranges::all_of(readyPlayers,
                               [](const auto& pair) { return pair.second; });
}

void
Lobby::decrementTTL()
{
    // This method is used to decrement the TTL of the lobby.
    // It will decrement the TTL of the lobby by 1.

    std::lock_guard lock(stateMutex);
    ttl--;
}

bool
Lobby::isLobbyDead()
{
    // This method is used to check if the lobby is empty.
    // It will return true if the lobby is empty, false otherwise.

    std::lock_guard lock(stateMutex);
    bool isDead = players.empty() && spectators.empty() && ttl <= 0;
    return isDead;
}

bool
Lobby::isLobbyPublic()
{
    // This method is used to check if the lobby is public.
    // It will return true if the lobby is public, false otherwise.

    std::lock_guard lock(stateMutex);
    return isPublic;
}

bool
Lobby::isPlayerInLobby(const std::string& sessionToken) const
{
    // This method is used to check if a player is in the lobby.
    // It will return true if the player is in the lobby, false otherwise.
    const bool inPlayers = players.contains(sessionToken);
    return inPlayers;
}

bool
Lobby::isSpectatorInLobby(const std::string& sessionToken) const
{
    // This method is used to check if a spectator is in the lobby.
    // It will return true if the spectator is in the lobby, false otherwise.
    const bool inSpectators = spectators.contains(sessionToken);
    return inSpectators;
}

bool
Lobby::isLobbyFull() const
{
    // This method is used to check if the lobby is full.
    // It will return true if the lobby is full, false otherwise.

    return static_cast<int>(players.size()) >= maxPlayers;
}

StatusCode
Lobby::initializeSocket()
{
    // This method is used to initialize the socket of the lobby.
    // It will return an error code if there is an error initializing the
    // socket. It will return success if the socket is initialized successfully.

    // create the socket
    lobbySocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (lobbySocket < 0)
    {
        printMessage("Error creating socket", MessageType::ERROR);
        return StatusCode::ERROR_CREATING_SOCKET;
    }

    // set the socket options
    if (setSocketOptions() != StatusCode::SUCCESS)
    {
        printMessage("Error setting socket options", MessageType::ERROR);
        close(lobbySocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // bind the socket to the address
    if (bind(lobbySocket, reinterpret_cast<sockaddr*>(&lobbyAddr),
             sizeof(lobbyAddr)) < 0)
    {
        printMessage("Error binding socket", MessageType::ERROR);
        close(lobbySocket);
        return StatusCode::ERROR_BINDING_SOCKET;
    }

    return StatusCode::SUCCESS;
}

StatusCode
Lobby::setSocketOptions()
{
    // This method is used to set the socket options of the lobby.
    // It will return an error code if there is an error setting the socket
    // options. It will return success if the socket options are set
    // successfully.

    // set the socket options
    constexpr int opt = 1;

    if (setsockopt(lobbySocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0)
    {
        printMessage("Error setting SO_REUSEADDR: " +
                         std::string(strerror(errno)),
                     MessageType::CRITICAL);
        close(lobbySocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    if (setsockopt(lobbySocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) <
        0)
    {
        printMessage("Error setting SO_REUSEPORT: " +
                         std::string(strerror(errno)),
                     MessageType::CRITICAL);
        close(lobbySocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // and timeout options
    timeval timeout;
    timeout.tv_sec = LOBBY_TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    if (setsockopt(lobbySocket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) < 0)
    {
        printMessage("Error setting SO_RCVTIMEO: " +
                         std::string(strerror(errno)),
                     MessageType::CRITICAL);
        close(lobbySocket);
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // set the server address / port
    lobbyAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &lobbyAddr.sin_addr);
    lobbyAddr.sin_port = htons(static_cast<uint16_t>(port));

    return StatusCode::SUCCESS;
}

StatusCode
Lobby::listen()
{
    // This method is used to listen for incoming requests and handle them.
    // It will return an error code if there is an error receiving the request
    // or sending the response. It will return success if the request is handled
    // successfully.

    // create the buffer and the client address length
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[MAX_BUFFER_SIZE] = {};

    // set the lobby as listening
    std::lock_guard lock(listenMutex);

    while (true)
    {
        // first, we need to check if the lobby is still running
        {
            std::lock_guard lock_(runningMutex);
            if (!running)
            {
                break;
            }
        }

        // if the lobby is still running, we can receive the request

        const ssize_t recvLen = recvfrom(
            lobbySocket, buffer, MAX_BUFFER_SIZE, 0,
            reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
        if (recvLen < 0)
        {
            continue; // timeout
        }

        // handle the request
        std::string requestContent(buffer, recvLen);
        std::string responseContent = handleRequest(requestContent);

        // send the response
        const ssize_t sentLen = sendto(
            lobbySocket, responseContent.c_str(), responseContent.size(), 0,
            reinterpret_cast<struct sockaddr*>(&clientAddr), clientAddrLen);
        if (sentLen < 0)
        {
            continue; // ignore this, player will timeout and try again
        }
    }

    return StatusCode::SUCCESS;
}

void
Lobby::printMessage(const std::string& message, const MessageType msgtype) const
{
    // This method is used to print a message to the console.
    // It will print the message with the specified message type.
    // this will only print the message if the debug flag is set to true.

    if (!debug)
    {
        return;
    }

    const std::string lobbyIdentifier = "[LOBBY-" + lobbyID + "] ";
    std::string messageType;

    switch (msgtype)
    {
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

    const std::string messageToPrint = lobbyIdentifier + messageType + message;
    std::cout << messageToPrint << std::endl;
}

std::string
Lobby::handleRequest(const std::string& requestContent)
{
    // handle the request and return the response
    // the response will be sent back to the client.

    // First of all, we need to deserialize the request
    // and verify that it went well. If it didn't, we
    // return an error response to the client.

    ServerRequest request;
    try
    {
        request = ServerRequest::deserialize(requestContent);
    }
    catch (std::runtime_error& e)
    {
        printMessage("Error deserializing request: " + std::string(e.what()),
                     MessageType::ERROR);
        // we use the INVALID ID since we have no way of knowing the ID of the
        // request that failed (not a valid deserializable JSON string)
        return ServerResponse::ErrorResponse(
                   INVALID_ID, StatusCode::ERROR_DESERIALIZING_REQUEST)
            .serialize();
    }

    printMessage("Handling request [" + getServerMethodString(request.method) +
                     "]",
                 MessageType::INFO);

    // Then, we need to handle the request properly according to its method
    // called and return the response to the client.

    switch (request.method)
    {
    case ServerMethods::GET_CURRENT_LOBBY:
        return handleGetCurrentLobbyRequest(request).serialize();

    case ServerMethods::LEAVE_LOBBY:
        return handleLeaveLobbyRequest(request).serialize();

    case ServerMethods::READY:
        return handleReadyRequest(request).serialize();

    case ServerMethods::UNREADY:
        return handleUnreadyRequest(request).serialize();

    default:
        printMessage("Request [" + getServerMethodString(request.method) +
                         "] not implemented",
                     MessageType::ERROR);
        return ServerResponse::ErrorResponse(request.id,
                                             StatusCode::ERROR_NOT_IMPLEMENTED)
            .serialize();
    }
}

ServerResponse
Lobby::handleGetCurrentLobbyRequest(const ServerRequest& request)
{
    // handle the get current lobby request
    // return the response to the client

    // first, we get the state of the lobby
    // then, we return the state of the lobby to the client
    // simple as that lolw

    const LobbyState state = getState();
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS,
                                           state);
}

ServerResponse
Lobby::handleLeaveLobbyRequest(const ServerRequest& request)
{
    // handle the leave lobby request
    // return the response to the client

    // First, we try to remove the player / spectator from the lobby
    // If both fail, we return an error response to the client.
    // If one of them succeeds, we return success response to the client.

    const StatusCode retPlayer = removePlayer(request.params.at("token"));
    const StatusCode retSpectator = removeSpectator(request.params.at("token"));

    // wow, looking at this, I realize this is quite possibly the worst way I
    // could have written a "OR" logical statement keeping this for the lore
    if (!(retPlayer == StatusCode::SUCCESS ||
          retSpectator == StatusCode::SUCCESS))
    {
        printMessage("Client " + request.params.at("token") +
                         " could not be removed from the lobby",
                     MessageType::ERROR);
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_CLIENT_NOT_IN_LOBBY);
    }

    // if we get here, we can return a success response
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS);
}

ServerResponse
Lobby::handleReadyRequest(const ServerRequest& request)
{
    // handle the ready request
    // return the response to the client

    // First, we check if the player is in the lobby, if they are not, we return
    // an error response. If the player is a spectator, we return an error
    // response. If the player is already ready, we return an error response.

    if (isSpectatorInLobby(request.params.at("token")))
    {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_SPECTATOR_CANNOT_READY);
    }
    else if (!isPlayerInLobby(request.params.at("token")))
    {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_CLIENT_NOT_IN_LOBBY);
    }
    else if (readyPlayers[request.params.at("token")])
    {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_PLAYER_ALREADY_READY);
    }

    // if we get here, we can set the player as ready
    readyPlayers[request.params.at("token")] = true;
    printMessage("Player " + request.params.at("token") + " is ready",
                 MessageType::INFO);
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS);
}

ServerResponse
Lobby::handleUnreadyRequest(const ServerRequest& request)
{
    // handle the unready request
    // return the response to the client

    // First, we check if the player is in the lobby, if they are not, we return
    // an error response. If the player is a spectator, we return an error
    // response. If the player is not ready, we return an error response.

    if (isSpectatorInLobby(request.params.at("token")))
    {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_SPECTATOR_CANNOT_READY);
    }
    else if (!isPlayerInLobby(request.params.at("token")))
    {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_CLIENT_NOT_IN_LOBBY);
    }
    else if (!readyPlayers[request.params.at("token")])
    {
        return ServerResponse::ErrorResponse(
            request.id, StatusCode::ERROR_PLAYER_NOT_READY);
    }

    // if we get here, we can set the player as unready
    readyPlayers[request.params.at("token")] = false;
    printMessage("Player " + request.params.at("token") + " is unready",
                 MessageType::INFO);
    return ServerResponse::SuccessResponse(request.id, StatusCode::SUCCESS);
}
