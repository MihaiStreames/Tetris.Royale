#ifndef VIEWTESTDATA_H
#define VIEWTESTDATA_H

#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

//#include "tetromino.hpp"
//#include "types.hpp"

using TempMatrix = std::vector<std::vector<int>>;

struct PlayerScore {
    int rank;
    std::string name;
    int score;
};

struct Notification {
    std::string message;
};

struct ChatMessage {
    std::string from;
    std::string text;
};

struct Lobby {
    const std::string lobbyID;
    std::string gameMode;
    int nbPlayers;
    int nbSpectators;
    std::string status;
};

struct GameMode {
    std::unordered_map<std::string, std::string> descriptions{
        {"ClassicButton", "Classic Tetris mode."},
        {"DuelButton", "1v1 classic Tetris (2 players)"},
        {"RoyaleButton", "Battle Royale Tetris with abilities, last alive wins. (3-9 players)"}
    };
};


/// Store all placeholder data here; in real usage, populate these from your Client class.
struct TestData {
    // Title
    std::string gameTitle = "TETRIS ROYALE";

    // Leaderboard
    std::vector<PlayerScore> leaderboardData{
        {1, "Player1", 100000},
        {2, "Player2", 80000},
        {3, "Player3", 60000},
    };

    // Notifications
    std::vector<Notification> notifications{
        {"PlayerX sent you a game invite!"},
        {"PlayerY sent you a game invite!"},
    };

    // Friend list
    std::vector<std::string> friendList{
        "Player1", "Player2", "Player3", "Player4",
        "Player5", "Player6", "Player7", "Player8"
    };

    // Conversations: each friend has a sequence of messages
    std::map<std::string, std::vector<ChatMessage> > conversations{
        {"Player1", {{"Player1", "Hello!"}, {"Me", "Hi Player1!"}}},
        {"Player2", {{"Player2", "Hello!"}, {"Me", "Hi Player2!"}}},
        {"Player3", {{"Player3", "Hello!"}, {"Me", "Hi Player3!"}}},
        {"Player4", {{"Player4", "Hello!"}, {"Me", "Hi Player4!"}}},
    };

    // (Login/Register)
    std::string username;
    std::string password;
    std::string passwordConfirm;

    // Example: Key bindings or settings
    std::map<std::string, std::string> keyBindings{
        {"Left", "\u2190"},
        {"Right", "\u2192"},
        {"Rotate", "\u2191"},
        {"Drop", "space"},
        {"Switch Player", "S"},
        {"Power-up", "Q"}
    };

    // In-game placeholders
    int playerScore = 100;
    int playerEnergy = 30;
    int spectatorCount = 0; // For "Spectator: X"
    std::string currentTarget = "Player 2";

    // Board
    TempMatrix tempBoard = {
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
     };

    /*Tetromino holdTetromino = Tetromino(PieceType::I);
    Tetromino nextTetromino = Tetromino(PieceType::O);*/

    // Lobby
    std::vector<Lobby> lobbies{
        {"ABC123", "Classic", 2, 0, "Readying Up"},
        {"DEF456", "Royale", 3, 2, "Ongoing"}
    };
    
    // Lobby
    std::string lobbyName = "ABC123";

    
    std::map<std::string, std::string> players{
        {"Player1" , "Ready"},
        {"Player2", "Not Ready"}
    };

    std::vector<std::string> spectators{
        "Spectator1", "Spectator2"
    };

    
};

/// Extern to be defined in a .cpp or used directly as a global.
extern TestData testData;

#endif //VIEWTESTDATA_H
