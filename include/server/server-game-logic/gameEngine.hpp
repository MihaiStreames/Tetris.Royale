#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "royalGame.hpp"
#include "tetrisGame.hpp"


class GameEngine {
public:
    virtual ~GameEngine() = default;

    GameEngine() = default;

    virtual bool handleAction(RoyalGame &game, Action action);

    virtual bool handleBag(TetrisGame &game);

    virtual bool handleFallingPiece(TetrisGame &game);

    virtual bool handlePlacingPiece(RoyalGame &game);

    virtual void handleSpawn(TetrisGame &game);

    virtual void handleSpawn(TetrisGame &game, Tetromino &piece);

    virtual void handleGameLogic(RoyalGame &game);

    virtual void handleGameOver(TetrisGame & /*eventually have game*/);

    virtual void handleScore(TetrisGame &game, int linesCleared);

    virtual void handlingRoutine(RoyalGame &game, Action action);

    virtual bool handleBonus(RoyalGame &game) { return false; }

    virtual bool handleMalus(RoyalGame &game) { return false; }

    static void sendToEnemy() { std::cerr << "You cannot access sendToEnemy from GameEngine"; }

    static void handleEnergy() { std::cerr << "You cannot access handleEnergy from GameEngine"; }
};

