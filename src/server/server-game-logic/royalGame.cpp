
#include "royalGame.hpp"



RoyalGame::RoyalGame(const int gWidth, const int gHeight, const int gScore, const int fc, const int lvl, const int totLinesCleared) :
    ClassicGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared) {

    // this is the constructor of the RoyalGame class
    // might need to modify this later (reverse controls ?)


}


void RoyalGame::spawnThunderStrike() {

    // this method is called when the player uses the thunder strike power-up
    // it will destroy a 2x2 area of blocks in a random column

    // find some column to destroy
    const int col = rand() % gameMatrix.getWidth();

    // this is the default y value the thunder will strike.
    // will remain -1 if no block is found in the column "col"
    int impactRow = gameMatrix.findHighestBlockInColumn(col);

    // if no block was found in the column, return
    if (impactRow == -1) { return; }
    else { gameMatrix.destroyAreaAroundBlock({col, impactRow}, THNUDERSTRIKE_BLAST_RADIUS); }

}


void RoyalGame::increaseFallingSpeed() {

    // this method is called when the player uses the speed power-up
    // it will make the pieces fall faster

    speedFactor++;

}

void RoyalGame::decreaseFallingSpeed() {

    // this method is called when the player uses the slow power-up
    // it will make the pieces fall slower

    speedFactor--;

    // if the speed factor is negative, set it to 0
    if (level + speedFactor < 0) {
        speedFactor = 0;
    }

}

void RoyalGame::pushSingleBlock() {

    // this method is called when the player uses the single block power-up
    // it will push 'SINGLE_BLOCKS_TO_PUSH' (2) single blocks to the player's board

    for (int i = 0; i < SINGLE_BLOCKS_TO_PUSH; ++i) { factory.pushPiece(Tetromino(PieceType::Single)); }

}

void RoyalGame::startDarkMode() {

    // this method is called when the player uses the dark mode power-up
    // it will make the game harder by making the board invisible

    // this is a CHRONO, meaning it will calculate a TIMESTAMP to stop the darkmode on.

    setDarkModeTimer(getFrameCount() + DARK_MODE_TIMER);
    setDarkModeFlag(true);
    

}

void RoyalGame::startBlockControls() {

    // this method is called when the player uses the block controls power-up
    // it will make the controls of the player's board inverted

    setBlockControlsFlag(true);

}

void RoyalGame::startInvertedControls() {

    // this method is called when the player uses the inverted controls power-up
    // it will make the controls of the player's board inverted

    setReverseControlsFlag(true);

}

