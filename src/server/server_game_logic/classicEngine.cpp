#include "classicEngine.hpp"
#include "classicGame.hpp"




void ClassicEngine::handleBasicPenalty(ClassicGame& game, const int linesCleared) {
    

    if (linesCleared <= 1) { return; }

    const std::vector<ClassicGame*>& opponents = game.getOpponents();

    // fetch the game of selected opponent
    ClassicGame* opponent = game.getTarget();

    // calculate the number of lines to add
    int linesToAdd;
    if (linesCleared < 4){
        linesToAdd = linesCleared - 1;
    } else {
        linesToAdd = 4;
    }

    // add the penalty lines to the opponent
    opponent->addPenaltyLines(linesToAdd);

}


void ClassicEngine::handleGameLogic(TetrisGame& game) {

    // clear the lines and calculate the score for the player
    const int linesCleared = game.getGameMatrix().clearFullLines();
    handleScore(game, linesCleared);

    // handle the penalty lines for the opponents
    auto* classicGame = dynamic_cast<ClassicGame*>(&game);
    handleBasicPenalty(*classicGame, linesCleared);

    // handle the spawn of the next piece if needed
    handleSpawn(game);

    // check if the game is over
    if (game.isGameOver()) { handleGameOver(game); }

}

