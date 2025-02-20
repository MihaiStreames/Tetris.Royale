#pragma once 

#include "tetrisGame.hpp"

class PowerUps{

    public:

        static void blocks_1x1(TetrisGame& game){
            for (int i = 0; i < 2; ++i){
                game.getFactory().pushPiece(Tetromino({0,0}, Single));
            }
        }

};