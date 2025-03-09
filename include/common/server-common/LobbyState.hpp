#ifndef LOBBY_STATE_HPP
#define LOBBY_STATE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "common.hpp"


struct LobbyState {

    std::string lobbyID;
    int port;
    int maxPlayers;
    GameMode gameMode;
    bool isPublic;
    
    std::unordered_map<std::string, std::string> players;
    std::unordered_map<std::string, bool> readyPlayers;
    std::unordered_map<std::string, std::string> spectators;

    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] static LobbyState deserialize(const std::string& data);
    [[nodiscard]] static LobbyState generateEmptyState();

};

#endif
