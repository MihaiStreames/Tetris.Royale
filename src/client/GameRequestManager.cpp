#include "GameRequestManager.hpp"

GameRequestManager::GameRequestManager(const std::string& serverIP,
                                       int lobbyServerPort)
    : serverIP(serverIP), lobbyServerPort(lobbyServerPort)
{
    // this is the constructor of the GameRequestManager class
    // it initializes the RequestManager session with the given serverIP and
    // lobbyServerPort all of them can be ignored and will then be set up to
    // their default values (common.hpp) the clientSocket is set to -1, which
    // means that the client is not connected to any server yet

    clientSocket = NO_FILE_DESCRIPTOR;
}

GameRequestManager::~GameRequestManager()
{
    // this is the destructor of the GameRequestManager class
    // it closes the clientSocket if it is connected to a server
    // this is done to avoid memory leaks and to free the resources used by the
    // clientSocket

    (void)disconnectFromServer();
}

std::string GameRequestManager::getServerIP()
{
    // this method is used to get the server IP
    // it will return the server IP

    return serverIP;
}

int GameRequestManager::getPort()
{
    // this method is used to get the port
    // it will return the port

    return lobbyServerPort;
}

StatusCode
GameRequestManager::connectToServer()
{
    // this method is used to connect to the server
    // it will create a socket and connect to the server

    // create the socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        return StatusCode::ERROR_CREATING_SOCKET;
    }

    // set the socket options
    if (setSocketOptions() != StatusCode::SUCCESS)
    {
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    return StatusCode::SUCCESS;
}

StatusCode
GameRequestManager::disconnectFromServer()
{
    // this method is used to disconnect from the server
    // it will close the socket

    // close the socket
    if (clientSocket != NO_FILE_DESCRIPTOR)
    {
        close(clientSocket);
        clientSocket = NO_FILE_DESCRIPTOR;
    }

    return StatusCode::SUCCESS;
}

// status thing

ServerResponse
GameRequestManager::getPlayerStatus(const std::string& username)
{
    // this method is used to get some status from a player having the username
    // [username]

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::GET_PLAYER_STATUS;
    request.params["username"] = username;

    // this is a special case, we want to send the request specifically to the
    // lobby server, so we grab the current port, restore the listening port,
    // send the request, and then restore the port

    const int currentPort = getCurrentPort();
    if (changePort(lobbyServerPort) != StatusCode::SUCCESS)
    {
        return ServerResponse::ErrorResponse(INVALID_ID,
                                             StatusCode::ERROR_CHANGING_PORT);
    }

    // send the request and receive the response
    (void)sendRequest(request);
    const ServerResponse response = receiveResponse();

    // restore the port
    if (changePort(currentPort) != StatusCode::SUCCESS)
    {
        return ServerResponse::ErrorResponse(INVALID_ID,
                                             StatusCode::ERROR_RESTORING_PORT);
    }

    return response;
}

// main menu stuff

ServerResponse
GameRequestManager::startSession(const std::string& username)
{
    // this method is used to start a new session
    // it will send a request to the lobby server to start a new session
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::START_SESSION;
    request.params["username"] = username;

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

ServerResponse
GameRequestManager::endSession(const std::string& token)
{
    // this method is used to end the current session
    // it will send a request to the lobby server to end the current session
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::END_SESSION;
    request.params["token"] = token;

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

ServerResponse
GameRequestManager::getPublicLobbiesList()
{
    // this method is used to get the list of public lobbies
    // it will send a request to the lobby server to get the list of public
    // lobbies it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::GET_PUBLIC_LOBBIES;

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

ServerResponse
GameRequestManager::createAndJoinLobby(const std::string& token,
                                       GameMode gameMode, int maxPlayers,
                                       bool isPublic)
{
    // this method is used to create and join a lobby
    // it will send a request to the lobby server to create and join a lobby
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::CREATE_LOBBY;
    request.params["token"] = token;
    request.params["gameMode"] = std::to_string(static_cast<int>(gameMode));
    request.params["maxPlayers"] = std::to_string(maxPlayers);
    request.params["visibility"] = isPublic ? "public" : "private";

    // send the request
    (void)sendRequest(request);
    ServerResponse response = receiveResponse();

    // if the request is sucessful, we join the lobby. if not, we can return the
    // response
    if (response.status != StatusCode::SUCCESS)
    {
        return response;
    }

    // we extract the lobbyState from the response
    LobbyState lobbyState = LobbyState::deserialize(response.data.at("lobby"));

    // we can join the lobby now
    return joinLobby(token, lobbyState.lobbyID);
}

ServerResponse
GameRequestManager::joinLobby(const std::string& token,
                              const std::string& lobbyID)
{
    // this method is used to join a lobby
    // it will send a request to the lobby server to join a lobby
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::JOIN_LOBBY;
    request.params["token"] = token;
    request.params["lobbyID"] = lobbyID;

    // send the request
    (void)sendRequest(request);
    ServerResponse response = receiveResponse();

    // change the port to the lobby port if the request is successful
    if (response.status != StatusCode::SUCCESS)
    {
        return response;
    }

    const int lobbyPort = std::stoi(response.data.at("port"));
    if (changePort(lobbyPort) != StatusCode::SUCCESS)
    {
        return ServerResponse::ErrorResponse(INVALID_ID,
                                             StatusCode::ERROR_CHANGING_PORT);
    }

    return response;
}

ServerResponse
GameRequestManager::spectateLobby(const std::string& token,
                                  const std::string& lobbyID)
{
    // this method is used to spectate a lobby
    // it will send a request to the lobby server to spectate a lobby
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::SPECTATE_LOBBY;
    request.params["token"] = token;
    request.params["lobbyID"] = lobbyID;

    // send the request
    (void)sendRequest(request);
    ServerResponse response = receiveResponse();

    // change the port to the lobby port if the request is successful
    if (response.status != StatusCode::SUCCESS)
    {
        return response;
    }

    const int lobbyPort = std::stoi(response.data.at("port"));
    if (changePort(lobbyPort) != StatusCode::SUCCESS)
    {
        return ServerResponse::ErrorResponse(INVALID_ID,
                                             StatusCode::ERROR_CHANGING_PORT);
    }

    return response;
}

// lobby stuff

ServerResponse
GameRequestManager::getCurrentLobbyState(const std::string& token)
{
    // this method is used to get the state of the current lobby
    // it will send a request to the lobby server to get the state of the
    // current lobby it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::GET_CURRENT_LOBBY;
    request.params["token"] = token;

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

ServerResponse
GameRequestManager::leaveLobby(const std::string& token)
{
    // this method is used to leave the current lobby
    // it will send a request to the lobby server to leave the current lobby
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::LEAVE_LOBBY;
    request.params["token"] = token;

    // send the request
    (void)sendRequest(request);
    ServerResponse response = receiveResponse();

    // restore the port if the request is successful
    if (response.status != StatusCode::SUCCESS)
    {
        return response;
    }

    // cool ternary operator
    return restoreListeningPort() == StatusCode::SUCCESS
               ? response
               : ServerResponse::ErrorResponse(
                     INVALID_ID, StatusCode::ERROR_RESTORING_PORT);
}

ServerResponse
GameRequestManager::readyUp(const std::string& token)
{
    // this method is used to ready up in the current lobby
    // it will send a request to the lobby server to ready up in the current
    // lobby it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::READY;
    request.params["token"] = token;

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

ServerResponse
GameRequestManager::unreadyUp(const std::string& token)
{
    // this method is used to unready up in the current lobby
    // it will send a request to the lobby server to unready up in the current
    // lobby it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::UNREADY;
    request.params["token"] = token;

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

// game stuff

ServerResponse
GameRequestManager::sendKeyStroke(const std::string& token,
                                  const KeyStrokePacket& keyStroke)
{
    // this method is used to send a key stroke to the game server
    // it will send a request to the game server to send a key stroke
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::KEY_STROKE;
    request.params["token"] = token;
    request.params["keystroke"] = keyStroke.serialize();

    // send the request
    (void)sendRequest(request);
    return receiveResponse();
}

ServerResponse
GameRequestManager::getGameState(const std::string& token)
{
    // this method is used to get the state of the game
    // it will send a request to the game server to get the state of the game
    // it will return the response from the server

    // create the request
    ServerRequest request;
    request.id = generateRequestID();
    request.method = ServerMethods::GET_GAME_STATE;
    request.params["token"] = token;

    // send the request

    (void)sendRequest(request);
    return receiveResponse();
}

// connectivity

StatusCode
GameRequestManager::sendRequest(const ServerRequest& request)
{
    // this method is used to send a request to the server
    // it will serialize the request and send it to the server

    // serialize the request
    const std::string serializedRequest = request.serialize();
    // send the request
    const ssize_t sendLen = sendto(
        clientSocket, serializedRequest.c_str(), serializedRequest.size(), 0,
        reinterpret_cast<struct sockaddr*>(&serverAddress),
        sizeof(serverAddress));
    if (sendLen < 0)
    {
        return StatusCode::ERROR_SENDING_REQUEST;
    }

    return StatusCode::SUCCESS;
}

ServerResponse
GameRequestManager::receiveResponse()
{
    // this method is used to receive a response from the server
    // it will receive the response from the server and deserialize it

    // receive the response
    char buffer[MAX_BUFFER_SIZE];
    const ssize_t recvLen = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
    if (recvLen < 0)
    {
        return ServerResponse::ErrorResponse(
            INVALID_ID, StatusCode::ERROR_RECEIVING_RESPONSE);
    }

    // deserialize the response
    const std::string serializedResponse(buffer, recvLen);
    return ServerResponse::deserialize(serializedResponse);
}

StatusCode
GameRequestManager::setSocketOptions()
{
    // this method is used to set the socket options
    // it will set the socket options for the client socket

    // set the socket options
    timeval timeout;
    timeout.tv_sec = CLIENT_TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) < 0)
    {
        return StatusCode::ERROR_SETTING_SOCKET_OPTIONS;
    }

    // set the server address / port
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr);
    serverAddress.sin_port = htons(static_cast<uint16_t>(lobbyServerPort));

    return StatusCode::SUCCESS;
}

int
GameRequestManager::getCurrentPort()
{
    // this method is used to get the current port of the server
    // it will return the port used by the current clientSocket

    if (clientSocket == NO_FILE_DESCRIPTOR)
    {
        return EMPTY_LOBBY_PORT;
    }

    return ntohs(serverAddress.sin_port);
}

StatusCode
GameRequestManager::changePort(int newPort)
{
    // this method is used to change the port of the server
    // it will change the port of the server address

    if (newPort == getCurrentPort())
    {
        // do nothing if the port is already the same
        return StatusCode::SUCCESS;
    }

    if (newPort <= 0 || newPort > MAX_PORT)
    {
        return StatusCode::ERROR_INVALID_PORT;
    }

    // change the port
    serverAddress.sin_port = htons(static_cast<uint16_t>(newPort));

    return StatusCode::SUCCESS;
}

StatusCode
GameRequestManager::restoreListeningPort()
{
    // this method is used to restore the listening socket
    // it will change the port of the server address to the lobby server port

    // change the port
    return changePort(lobbyServerPort);
}

int
GameRequestManager::generateRequestID()
{
    // this method is used to generate a request ID
    // it will return a random number between 0 and MAX_REQUEST_ID

    return rand() % MAX_REQUEST_ID;
}
