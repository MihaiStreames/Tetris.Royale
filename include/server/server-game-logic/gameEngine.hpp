#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include "tetrisGame.hpp"



class GameEngine {

public:
    
    GameEngine() {}

    virtual bool handleAction(TetrisGame& game, const Action action);

    virtual bool handleBag(TetrisGame& game);

    virtual bool handleFallingPiece(TetrisGame& game);

    virtual bool handlePlacingPiece(TetrisGame& game);

    virtual void handleSpawn(TetrisGame& game);

    virtual void handleSpawn(TetrisGame &game, Tetromino &piece);

    virtual void handleGameLogic(TetrisGame& game) ;

    virtual void handleGameOver(TetrisGame& /*eventually have game*/);

    virtual void handleScore(TetrisGame& game, const int linesCleared);

    virtual void handlingRoutine(TetrisGame& game, const Action action);
    
    virtual bool handleBonus(TetrisGame &game);

    virtual bool handleMalus(TetrisGame &game);

    virtual void sendToEnemy(int linesCleared, TetrisGame& enemyGame);

    virtual void handleEnergy(TetrisGame &game, const int linesCleared);

};

