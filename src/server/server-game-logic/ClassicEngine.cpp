
#include "ClassicEngine.hpp"

void
ClassicEngine::handleBasicPenalty(ClassicGame& game, const int linesCleared)
{
    // this should handle the line penalty for the opponents
    // the penalty is calculated based on the number of lines cleared

    if (linesCleared <= 1)
    {
        // no penalty lines to add for 0 or 1 lines cleared
        return;
    }

    // get the target
    ClassicGame* opponent = static_cast<ClassicGame*>(game.getTarget());

    if (opponent == nullptr)
    {
        // no opponent to add penalty lines to
        return;
    }

    // calculate the number of lines to add
    int linesToAdd;

    if (linesCleared < MAX_COMBO)
    {
        linesToAdd = linesCleared - 1;
    }
    else
    {
        linesToAdd = MAX_COMBO;
    }

    // add the penalty lines to the opponent
    opponent->addPenaltyLines(linesToAdd);
}

void
ClassicEngine::handleGameLogic(TetrisGame& game)
{
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
    if (classicGame.isGameOver())
    {
        handleGameOver(classicGame);
    }
}

bool
ClassicEngine::viewPreviousOpponent(TetrisGame& game)
{
    // this should handle the view of the previous opponent
    // the view should be changed to the previous opponent in the list of
    // opponents

    ClassicGame& classicGame = static_cast<ClassicGame&>(game);

    // get the index of the previous opponent
    int idx = classicGame.getTargetIndex() - 1;

    // change the player view to the previous opponent
    classicGame.changePlayerView(idx);

    return true;
}

bool
ClassicEngine::viewNextOpponent(TetrisGame& game)
{
    // this should handle the view of the next opponent
    // the view should be changed to the next opponent in the list of opponents

    ClassicGame& classicGame = static_cast<ClassicGame&>(game);

    // get the index of the next opponent
    int idx = classicGame.getTargetIndex() + 1;

    // change the player view to the next opponent
    classicGame.changePlayerView(idx);

    return true;
}
