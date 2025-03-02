#include "classicGame.hpp"

std::vector<ClassicGame*> ClassicGame::getOpponents() { return opponents;}
ClassicGame* ClassicGame::getTarget() { return target; }

void ClassicGame::addPenaltyLines(int linesToAdd){
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

void ClassicGame::changePlayerView() {
     if (targetIndex < opponents.size()) {
        targetIndex += 1;
    } else {
        targetIndex = 0;
    }
    target = opponents[targetIndex];
}