
#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "Common.hpp"
#include "Types.hpp"

#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct GameState
{
  public:
    std::string playerUsername;
    tetroMat playerGrid;
    PieceType nextTetro;
    PieceType holdTetro;
    bool isGameOver;
    GameMode gameMode; 

    virtual ~GameState() = default;
    [[nodiscard]] virtual std::string serialize() const = 0;
};

struct SpectatorState : public GameState
{
  public:
    [[nodiscard]] static SpectatorState generateEmptyState();
    [[nodiscard]] std::string serialize() const override;
    [[nodiscard]] static SpectatorState deserialize(const std::string& data);
};

struct PlayerState : public GameState
{
  public:
    int playerScore;
    int playerLevel;
    int playerLines;
    int playerEnergy = -1;
    std::string targetUsername;
    tetroMat targetGrid;

    [[nodiscard]] static PlayerState generateEmptyState();
    [[nodiscard]] std::string serialize() const override;
    [[nodiscard]] static PlayerState deserialize(const std::string& data);
};

#endif