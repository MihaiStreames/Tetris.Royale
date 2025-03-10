#include "GameServer.hpp"

GameServer::GameServer(const std::string& ip,
                       const std::shared_ptr<LobbyServer>& lobbyServer,
                       const bool debug)
    : ip(ip), lobbyServer(lobbyServer), debug(debug)
{
    // this is the constructor for the GameServer class
    // this will be used to create a new game server instance, using the
    // lobbyServer data to initialize the game server
}

GameServer::~GameServer()
{
    // this is the destructor for the GameServer class
    // this will be used to close the game server and free the resources
    // std::lock_guard<std::mutex> lock(runningMutex);
    // if (running) { (void) closeGameServer(); }
}

StatusCode
GameServer::startGameServer()
{
    // This method is used to start the game server. It should be called in a
    // new thread. It will start the game server and open the socket for the
    // players to connect. this will also start the game server loop, which will
    // run until the game server is closed.

    printMessage("Game server starting on " + ip, MessageType::INFO);

    // set the running flag to true
    {
        std::lock_guard lock(runningMutex);
        running = true;
    }

    // thread to listen for ready lobbies and start games
    listenThread = std::thread(&GameServer::listen, this);

    return StatusCode::SUCCESS;
}

StatusCode
GameServer::closeGameServer()
{
    // This method is used to close the game server. It should be called in a
    // new thread. It will close the game server and close the socket for the
    // players to connect. this will also stop the game server loop, which will
    // run until the game server is closed.

    // set the running flag to false
    {
        std::lock_guard lock(runningMutex);
        running = false;
    }

    // then, we wait for the listen function to finish
    {
        std::lock_guard lock(listenMutex);
    }

    // then, we wait for the listen function to finish (if it is listening)
    // and close all the games
    {
        std::lock_guard lock(gamesMutex);
        for (const auto& game : activeGames)
        {
            (void)game->closeGame();
        }
        activeGames.clear();
    }

    // and finally join the listen thread if it is still running
    if (listenThread.joinable())
    {
        listenThread.join();
    }

    printMessage("Game server closed", MessageType::INFO);
    return StatusCode::SUCCESS;
}

bool
GameServer::isSessionInAnyGame(const std::string& token)
{
    // This method is used to check if a session is in any game.
    // It will return true if the session is in any game, and false otherwise.

    std::lock_guard lock(gamesMutex);
    for (const auto& game : activeGames)
    {
        if (game->isSessionInGame(token))
        {
            return true;
        }
    }

    return false;
}

int
GameServer::countGames()
{
    // This method is used to count the number of games in the game server.
    // It will return the number of games.

    std::lock_guard lock(gamesMutex);
    return static_cast<int>(activeGames.size());
}

bool
GameServer::isRunning()
{
    // This method is used to check if the game server is running.
    // It will return true if the game server is running, and false otherwise.

    std::lock_guard lock(runningMutex);
    return running;
}

void
GameServer::listen()
{
    // This method is used to listen for ready lobbies and start games.
    // It will run in a separate thread and will start the games when the
    // lobbies are ready.

    std::lock_guard lock(listenMutex);

    while (true)
    {
        // first, we need to check if the game server is still running
        {
            std::lock_guard lock(runningMutex);
            if (!running)
            {
                break;
            }
        }

        // if the game server is still running, we can get the ready lobbies and
        // the empty lobbies
        std::vector<std::shared_ptr<Lobby>> readyLobbies =
            lobbyServer->getReadyLobbies();
        std::vector<std::shared_ptr<Lobby>> deadLobbies =
            lobbyServer->getDeadLobbies();

        // we kill the dead lobbies
        for (const auto& lobby : deadLobbies)
        {
            if (lobbyServer->closeLobby(lobby->getLobbyID()) !=
                StatusCode::SUCCESS)
            {
                printMessage("Error closing lobby: " +
                                 lobby->getState().lobbyID,
                             MessageType::ERROR);
            }
        }

        // then, we start the games
        for (const auto& lobby : readyLobbies)
        {
            // check if we can start a new game
            if (countGames() < MAX_GAMES)
            {
                startGame(lobby);
            }
            else
            {
                printMessage(
                    "Cannot start a new game, maximum number of games reached",
                    MessageType::WARNING);
                break;
            }
        }
    }

    return;
}

void
GameServer::startGame(const std::shared_ptr<Lobby>& lobby)
{
    // This method is used to start a game.
    // It will create a new game instance and start the game.

    std::lock_guard lock(gamesMutex);

    // create the game
    LobbyState lobbyState = lobby->getState();
    const auto game = std::make_shared<Game>(ip, lobbyState, debug);

    // close the lobby
    if (lobbyServer->closeLobby(lobbyState.lobbyID) != StatusCode::SUCCESS)
    {
        printMessage("Error closing lobby: " + lobbyState.lobbyID,
                     MessageType::ERROR);
        return;
    }

    // start the game
    const StatusCode gameStarted = game->startGame();
    if (gameStarted != StatusCode::SUCCESS)
    {
        printMessage("Error starting game: " + getStatusCodeString(gameStarted),
                     MessageType::ERROR);
        return;
    }

    // add the game to the list of active games
    activeGames.push_back(game);

    return;
}

void
GameServer::printMessage(const std::string& message, MessageType msgType) const
{
    // This method is used to print a message to the console.
    // It will print the message with the specified message type
    // only if the debug flag is set to true.

    if (!debug)
    {
        return;
    }

    const std::string gameIdentifier = "[Game Server] ";
    std::string msgtype_str;

    switch (msgType)
    {
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

    std::cout << gameIdentifier << "[" << msgtype_str << "] " << message
              << std::endl;

    return;
}
