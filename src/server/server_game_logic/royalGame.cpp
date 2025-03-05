#include "royalGame.hpp"

void RoyalGame::inverted_command(bool flag) {
    reverseControls = flag;
}

void RoyalGame::thunder_strike() {

    // find some column to destroy
    int col = rand() % gameMatrix.getWidth();
    tetroMat& board = gameMatrix.getBoard();


    // this is the default y value the thunder will strike.
    // will remain -1 if no block is found in the column "col"
    int impactRow = -1;

    // find the first block in the column
    for (int y = 0; y < gameMatrix.getHeight(); ++y) {

        if (board[y][col] != 0) {
            impactRow = y;
            break;
        }

    }

    // if no block was found in the column, return
    if (impactRow == -1) { return; }


    // destroy the block and the blocks around it (2x2)
    // ?? this logic SHOULD NOT be here, it should be in the gameMatrix class
    
    for (int dy = 0; dy < 2; ++dy) {

        for (int dx = 0; dx < 2; ++dx) {

            int x = col + dx;
            int y = impactRow + dy;

            if (x < gameMatrix.getWidth() && y < gameMatrix.getHeight()) {                    
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

    // !! magic number (600)
    setDarkModeTimer(getFrameCount() + 600);
    setDarkMode(true);
    
}

void RoyalGame::blocs_1x1() {

    // !! this should be a CONSTANT at the very least, or some logic in the gameMatrix class
    // !! this is a magic number (2)

    for (int i = 0; i < 2; ++i){

        factory.pushPiece(Tetromino({0, 0}, Single));

    }

}

void RoyalGame::slow_falling_pieces() {

    // ?? why level 0 check here?
    if (level > 0) {

        speedFactor--;
        if (level + speedFactor < 0) {
            // ?? what does speedFactor = 0 do? pieces not falling?
            speedFactor = 0;
        }
    }

}

