#pragma once

#include <cstdlib>
#include "tetrisGame.hpp"

class ClassicGame : public TetrisGame
{
    // J'ai retiré le gameOver puisqu'on y a acces depuis son parent
    std::vector<ClassicGame *> opponents;

public:

    virtual std::vector<ClassicGame*> getOpponents() { return opponents;}

    virtual void addPenaltyLines(int linesToAdd){
        GameMatrix& matrix = getGameMatrix();
        tetroMat& board = matrix.getBoard();
        
        // If the pushed lines have tetrominos -> game over
        for (int i = 0; i < linesToAdd; ++i){
            for (int cell : getGameMatrix().getBoard()[i]){
                if (cell){
                    setGameOver(true);
                    return;
                }
            }
        }
        board.erase(board.begin(), board.begin() + linesToAdd);
        int holePosition = rand() % matrix.getWidth();

        for (int i = 0; i < linesToAdd; ++i) {
            std::vector<int> newLine(matrix.getWidth(), 8);
            newLine[holePosition] = 0;
            board.push_back(newLine);
        }
    }

    // permet de switch l'affichage entre les opposants
    void changePlayerView() {
    }

    void setGameOver(const bool flag) {
        gameOver = flag;
    }
};