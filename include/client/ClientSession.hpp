#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include "Common.hpp"
#include "DBRequestManager.hpp"
#include "GameRequestManager.hpp"
#include "GameState.hpp"

#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class ClientSession {
public:
    // public here
    ClientSession(const std::string &server_ip, int lobby_port, int db_port,
                  bool debug = false);

    ~ClientSession();

    DBRequestManager dbRequestManager;
    GameRequestManager gameRequestManager;

    // Getters for player info
    [[nodiscard]] std::string getServerIP();

    [[nodiscard]] int getLobbyPort();

    [[nodiscard]] int getDBPort();

    [[nodiscard]] std::string getUsername();

    [[nodiscard]] std::string getAccountID();

    [[nodiscard]] std::string getToken();

    [[nodiscard]] int getBestScore();

    void updateLocalMessages(const std::string &otherAccountID,
                             const ChatMessage &message);

    [[nodiscard]] std::vector<std::string> &getFriendList();

    [[nodiscard]] std::vector<std::string> &getPendingFriendRequests();

    std::string getAccountIDFromUsername(const std::string &username);

    std::string getFriendUsername(const std::string &friendID);

    std::string getRequestUsername(const std::string &requestID);

    // Setters for updating local session info
    void setUsername(const std::string &username);

    void setAccountID(const std::string &accountID);

    void setToken(const std::string &token);

    void setBestScore(int score);

    void setFriendList(const std::vector<std::string> &friends);

    void setPendingFriendRequests(const std::vector<std::string> &requests);

    // Database operations
    [[nodiscard]] StatusCode loginPlayer(const std::string &username, const std::string &password);

    [[nodiscard]] StatusCode registerPlayer(const std::string &username, const std::string &password);

    [[nodiscard]] std::vector<ChatMessage> getPlayerMessages(const std::string &otherAccountID);

    [[nodiscard]] std::vector<PlayerScore> getLeaderboard(int limit = 5);

    [[nodiscard]] StatusCode fetchPlayerData();

    [[nodiscard]] StatusCode updatePlayer(const std::string &newName, const std::string &newPassword);

    [[nodiscard]] StatusCode postScore(int score);

    [[nodiscard]] StatusCode sendMessage(const std::string &receiverID, const std::string &messageContent);

    // Friend-related operations
    [[nodiscard]] StatusCode sendFriendRequest(const std::string &receiverID);

    [[nodiscard]] StatusCode acceptFriendRequest(const std::string &senderID);

    [[nodiscard]] StatusCode declineFriendRequest(const std::string &senderID);

    [[nodiscard]] StatusCode removeFriend(const std::string &friendID);

    // ================== Game operations ================== //
    [[nodiscard]] ClientStatus getOwnStatus();

    [[nodiscard]] ClientStatus getPlayerStatus(const std::string &username);

    [[nodiscard]] StatusCode startSession();

    [[nodiscard]] StatusCode endSession();

    [[nodiscard]] std::unordered_map<std::string, std::string> getPublicLobbiesList();

    [[nodiscard]] StatusCode createAndJoinLobby(GameMode gameMode, int maxPlayers, bool isPublic);

    [[nodiscard]] StatusCode joinLobby(const std::string &lobbyID);

    [[nodiscard]] StatusCode spectateLobby(const std::string &lobbyID);

    [[nodiscard]] LobbyState getCurrentLobbyState();

    [[nodiscard]] StatusCode leaveLobby();

    [[nodiscard]] StatusCode readyUp();

    [[nodiscard]] StatusCode unreadyUp();

    [[nodiscard]] StatusCode sendKeyStroke(const Action &action);

    [[nodiscard]] PlayerState getPlayerState();

    [[nodiscard]] SpectatorState getSpectatorState();

private:
    // private here
    std::string username_;
    std::string accountID_;
    std::string token_;
    int bestScore_;
    std::vector<std::string> friendList_;
    std::vector<std::string> pendingFriendRequests_;
    std::unordered_map<std::string, std::string> friendIDToUsername;
    std::unordered_map<std::string, std::string> pendingRequestIDToUsername;
    std::unordered_map<std::string, std::vector<ChatMessage> > conversations_;
    bool debug_;
};

#endif
