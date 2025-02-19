#pragma once

#include "tetrisGame.hpp"

class ClassicDuelGame : public TetrisGame{
    bool gameOver = false;

public:
    void addLines() {
    }

    void sendInfo() {
    }

    // permet de switch l'affichage entre les opposants
    void changePlayerView() {
    }

    void setGameOver(const bool flag) {
        gameOver = flag;
        if (playerCount == 1) {
            
        }
    }
};