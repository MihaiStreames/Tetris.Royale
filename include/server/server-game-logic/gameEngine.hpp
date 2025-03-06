#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "royalGame.hpp"
#include "tetrisGame.hpp"


class GameEngine {

public:

    GameEngine() = default;
    virtual ~GameEngine() = default;


    [[nodiscard]] virtual bool handleAction(TetrisGame &game, Action action);

    [[nodiscard]] virtual bool handleBag(TetrisGame &game);
    virtual void handleEmptyBag(TetrisGame &game);
    virtual void handleSwap(TetrisGame &game);
    
    [[nodiscard]] virtual bool handleFallingPiece(TetrisGame &game);
    [[nodiscard]] virtual bool handlePlacingPiece(TetrisGame &game);

    virtual void handleSpawn(TetrisGame &game);
    virtual void handleSpawn(TetrisGame &game, Tetromino &piece);
    
    virtual void handleGameLogic(TetrisGame &game);
    virtual void handleGameOver(TetrisGame &game);
    virtual void handleScore(TetrisGame &game, int linesCleared);
    virtual void handlingRoutine(TetrisGame &game, Action action);


    // interface for bonus and malus (should raise an exception if not implemented)

    virtual void handleBonus(TetrisGame &game);
    virtual void handleMalus(TetrisGame &game);

    static void sendToEnemy();
    static void handleEnergy();

};


