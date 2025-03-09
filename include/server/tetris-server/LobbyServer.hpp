#ifndef LOBBY_SERVER_HPP
#define LOBBY_SERVER_HPP

#include <string>
#include <unordered_map>
#include <random>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Lobby.hpp"
#include "ServerRequest.hpp"
#include "ServerResponse.hpp"
#include "GameServer.hpp"
#include "common.hpp"


// forward declaration
class GameServer;


// LobbyServer class is used to manage the lobbies that are created by the clients.
// LobbyServer is also responsible for listening to the clients on the specified port
// and handle port allocation to the lobbies that are created.


class LobbyServer {

    public:

        LobbyServer(const std::string& IPAddr, int listenPort, bool debug = false);
        ~LobbyServer();
    
        [[nodiscard]] StatusCode startLobbyServer();
        [[nodiscard]] StatusCode closeLobbyServer();

        void setGameServer(std::shared_ptr<GameServer> gameServer);

        // session management
        [[nodiscard]] StatusCode addClientSession(const std::string& token, const std::string& username);
        [[nodiscard]] StatusCode removeClientSession(const std::string& token);
        [[nodiscard]] std::string getClientSessionUsername(const std::string& token) const;
        [[nodiscard]] std::string getClientSessionToken(const std::string& username) const;
        
        
        [[nodiscard]] std::shared_ptr<Lobby> getLobby(const std::string& lobbyID) const;  // maybe this needs to be private
        [[nodiscard]] int getLobbyPort(const std::string& lobbyID) const;
        [[nodiscard]] StatusCode closeLobby(const std::string& lobbyID);

        // data analysis mostly lol we want some stats to flex with pretty gui
        [[nodiscard]] int countPlayers() const;
        [[nodiscard]] int countLobbies() const;
        [[nodiscard]] bool isRunning();
        
        // this is called by the game server to get the lobbies that are ready
        [[nodiscard]] std::vector<std::shared_ptr<Lobby>> getReadyLobbies() const;
        [[nodiscard]] std::vector<std::shared_ptr<Lobby>> getDeadLobbies() const;
        [[nodiscard]] std::vector<std::shared_ptr<Lobby>> getPublicLobbies() const;
        

    private:

        [[nodiscard]] StatusCode listen();
        [[nodiscard]] StatusCode initializeSocket();
        [[nodiscard]] StatusCode setSocketOptions();
        
        // some utility stuff
        [[nodiscard]] std::unordered_map<std::string, std::shared_ptr<Lobby>> getLobbies() const;
        
        [[nodiscard]] bool isSessionActive(const std::string& token) const;
        [[nodiscard]] bool doesUserHaveSession(const std::string& username) const;
        void printMessage(const std::string& message, MessageType msgtype) const;

        // generation shit
        [[nodiscard]] static std::string generateToken(size_t length);
        [[nodiscard]] static std::string generateLobbyID(size_t length);

        // lobby management (allocating port and creating)
        [[nodiscard]] int findFreePort() const;
        void startLobby(const std::string& lobbyID) const;

        // requests handling
        [[nodiscard]] std::string handleRequest(const std::string& requestData);
        [[nodiscard]] ServerResponse handleStartSessionRequest(const ServerRequest& request);
        [[nodiscard]] ServerResponse handleEndSessionRequest(const ServerRequest& request);
        [[nodiscard]] ServerResponse handleGetLobbyRequest(const ServerRequest& request) const;
        [[nodiscard]] ServerResponse handleGetPublicLobbiesRequest(const ServerRequest& request);
        [[nodiscard]] ServerResponse handleCreateLobbyRequest(const ServerRequest& request);
        [[nodiscard]] ServerResponse handleJoinLobbyRequest(const ServerRequest& request) const;
        [[nodiscard]] ServerResponse handleSpectateLobbyRequest(const ServerRequest& request) const;
        [[nodiscard]] ServerResponse handleGetPlayerStatusRequest(const ServerRequest& request) const;
        

        // attributes

        std::string ip;
        int port;
        std::shared_ptr<GameServer> gameServer;  // the game server that is using this lobby server
        bool debug;
        bool running = false;

        int serverSocket;
        struct sockaddr_in serverAddr;
        struct sockaddr_in clientAddr;
        
        std::unordered_map<std::string, int> lobbies;                           // LOBBY ID -> PORT USED
        std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbyObjects;   // LOBBY ID -> LOBBY POINTER
        std::unordered_map<std::string, std::string> clientTokens;              // TOKEN -> USERNAME

        mutable std::mutex lobbiesMutex;        // protecting access to lobbies
        mutable std::mutex clientMutex;         // protecting access to clients
        std::mutex runningMutex;                // protecting access to running flag
        std::mutex listenMutex;                 // protecting access to listen function

        std::thread listenThread;

};
    

#endif
