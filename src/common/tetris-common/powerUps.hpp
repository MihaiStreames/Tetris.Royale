#pragma once 

#include "tetrisGame.hpp"

class PowerUps{
    private:
        int energy = 0;
        bool availablePowerUp = false;

    public:

        static void blocks_1x1(TetrisGame& game){
            for (int i = 0; i < 2; ++i){
                game.getFactory().pushPiece(Tetromino({0,0}, Single));
            }
        }

        static void fast_falling_pieces(TetrisGame& game){
            game.incrementLevel();
        }

};