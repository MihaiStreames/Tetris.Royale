
#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP


#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "common.hpp"
#include "types.hpp"


struct GameState {

    public:

        std::string playerUsername;
        tetroMat playerGrid;
        PieceType nextTetro;
        PieceType holdTetro;

        virtual ~GameState() = default;
        virtual std::string serialize() const = 0;

};


struct SpectatorState : public GameState {

    public:

        std::string serialize() const override;
        static std::unique_ptr<SpectatorState> deserialize(const std::string& data);

};


struct PlayerState : public GameState {

    public:

        int playerScore;
        int playerLevel;

        std::string targetUsername;
        tetroMat targetGrid;

        std::string serialize() const override;
        static std::unique_ptr<PlayerState> deserialize(const std::string& data);

};



#endif