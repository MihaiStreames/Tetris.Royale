#include "royalGame.hpp"

void RoyalGame::inverted_command(bool flag) {
    reverseControls = flag;
}

void RoyalGame::thunder_strike() {
    // Choisir une colonne qui sera frappée par la foudre
    int col = rand() % gameMatrix.getWidth();

    // -1 = colonne ou se trouve aucune brique
    int impactRow = -1;

    tetroMat& board = gameMatrix.getBoard();

    for (int y = 0; y < gameMatrix.getHeight(); ++y)
    {
        if (board[y][col] == 1)
        { // Trouve la première brique
            impactRow = y;
            break;
        }
    }

    // Si aucun bloc touché, on ne fait rien
    if (impactRow == -1)
        return ; 

    // Détruire la zone 2x2 autour de l'impact
    for (int dy = 0; dy < 2; ++dy)
    {
        for (int dx = 0; dx < 2; ++dx)
        {
            int x = col + dx;
            int y = impactRow + dy;
            if (x < gameMatrix.getWidth() && y < gameMatrix.getHeight())
            {                    
                board[y][x] = 0;
            }
        }
    }
}

void RoyalGame::block_command() {
    blockCommand = !blockCommand;
}

void RoyalGame::fast_falling_pieces() {
    speedFactor++;
}

void RoyalGame::darkMode() {
    setDarkModeTimer(getFrameCount() + 600);
    setDarkMode(true);
}

void RoyalGame::blocs_1x1() {
    for (int i = 0; i < 2; ++i){
        factory.pushPiece(Tetromino({0,0}, Single));
    }
}

void RoyalGame::slow_falling_pieces() {
    if (level > 0) {
        speedFactor--;
        if (level + speedFactor < 0) { // verifier si c est correct
            speedFactor = 0;
        }
    }
}