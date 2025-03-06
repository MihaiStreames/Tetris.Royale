
#include "classicEngine.hpp"


void ClassicEngine::handleBasicPenalty(ClassicGame& game, const int linesCleared) {
    
    // this should handle the line penalty for the opponents
    // the penalty is calculated based on the number of lines cleared

    if (linesCleared <= 1) {
        // no penalty lines to add for 0 or 1 lines cleared
        return;
    }

    // fetch the opponent of the player
    const std::vector<ClassicGame*>& opponents = game.getOpponents();
    ClassicGame* opponent = game.getTarget();

    // calculate the number of lines to add
    int linesToAdd;

    if (linesCleared < MAX_COMBO) {

        linesToAdd = linesCleared - 1;

    } else {

        linesToAdd = MAX_COMBO;

    }

    // add the penalty lines to the opponent
    opponent->addPenaltyLines(linesToAdd);

}


void ClassicEngine::handleGameLogic(TetrisGame &game) {

    // this should handle the game logic for the classic game mode
    // the game logic should handle the following:
    // - clear the full lines
    // - handle the score
    // - handle the penalty lines for the opponents
    // - handle the spawn of the next piece if needed
    // - check if the game is over

    ClassicGame& classicGame = static_cast<ClassicGame&>(game);

    // clear the full lines and handle the score
    const int linesCleared = classicGame.getGameMatrix().clearFullLines();
    handleScore(classicGame, linesCleared);

    // handle the penalty lines for the opponents
    handleBasicPenalty(classicGame, linesCleared);

    // handle the spawn of the next piece if needed
    handleSpawn(classicGame);

    // check if the game is over
    if (classicGame.isGameOver()) { handleGameOver(classicGame); }

}

