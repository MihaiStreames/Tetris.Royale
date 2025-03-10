#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DBRequestManager.hpp"
#include "GameRequestManager.hpp"
#include "GameState.hpp"
#include "common.hpp"


class ClientSession {

public:

    // public here
    ClientSession(bool debug = false);
    ~ClientSession();

    DBRequestManager dbRequestManager;
    GameRequestManager gameRequestManager;

    // Getters for player info
    [[nodiscard]] std::string getUsername();
    [[nodiscard]] std::string getAccountID();
    [[nodiscard]] std::string getToken();
    [[nodiscard]] int getBestScore();
    [[nodiscard]] std::vector<std::string>& getFriendList();
    [[nodiscard]] std::vector<std::string>& getPendingFriendRequests();

    // Setters for updating local session info
    void setUsername(const std::string &username);
    void setAccountID(const std::string &accountID);
    void setToken(const std::string &token);
    void setBestScore(int score);
    void setFriendList(const std::vector<std::string>& friends);
    void setPendingFriendRequests(const std::vector<std::string>& requests);

    // Database operations
    void loginPlayer(const std::string &username, const std::string &password);
    void registerPlayer(const std::string &username, const std::string &password);
    void fetchPlayerData();  // Get full player info from DB (username, bestScore, friend lists, etc.)
    void updatePlayer(const std::string &newName, const std::string &newPassword);
    void postScore(int score);

    // Friend-related operations
    void sendFriendRequest(const std::string &receiverID);
    void acceptFriendRequest(const std::string &senderID);
    void declineFriendRequest(const std::string &senderID);
    void removeFriend(const std::string &friendID);

    // ================== Game operations ================== //

    [[nodiscard]] ClientStatus getOwnStatus();
    [[nodiscard]] ClientStatus getPlayerStatus(const std::string& username);

    void startSession();
    void endSession();

    [[nodiscard]] std::unordered_map<std::string, std::string> getPublicLobbiesList();
    void createAndJoinLobby(GameMode gameMode, int maxPlayers, bool isPublic);
    void joinLobby(const std::string& lobbyID);
    void spectateLobby(const std::string& lobbyID);

    [[nodiscard]] LobbyState getCurrentLobbyState();
    void leaveLobby();
    void readyUp();
    void unreadyUp();

    void sendKeyStroke(const Action& action);
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
    bool debug_;

};


#endif
