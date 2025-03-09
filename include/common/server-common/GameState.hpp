
#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "common.hpp"


struct GameState {

    public:

        std::string sessionToken;
        std::unordered_map<std::string, Matrix> playerGrids;
        std::unordered_map<std::string, int> scores;

        virtual ~GameState() = default;
        virtual std::string serialize() const = 0;

};


struct SpectatorState : public GameState {

    public:

        [[nodiscard]] std::string serialize() const override;
        [[nodiscard]] static SpectatorState deserialize(const std::string& data);
        [[nodiscard]] static SpectatorState generateEmptyState();

};



#endif