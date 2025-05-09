#include "ClassicEngine.hpp"



bool ClassicEngine::handleAction(TetrisGame& game, Action action) {
    // this method is responsible for handling the game logic
    // based on the action that the player has taken

    // will throw an exception if the action is invalid
    // handled actions include : classic moves, special moves, malus, bonus and
    // none

    ClassicGame &royalGame = static_cast<ClassicGame&>(game);
    GameMatrix &gm = royalGame.getGameMatrix();

    switch (action) {
        // classic moves
        case Action::MoveLeft:
            return gm.tryMoveLeft();
        case Action::MoveRight:
            return gm.tryMoveRight();
        case Action::MoveDown:
            return gm.tryMoveDown();
        case Action::RotateLeft:
            return gm.tryRotateLeft();
        case Action::RotateRight:
            return gm.tryRotateRight();

        // see opponent moves
        case Action::SeePreviousOpponent:
            return viewPreviousOpponent(royalGame);
        case Action::SeeNextOpponent:
            return viewNextOpponent(royalGame);

        // special moves
        case Action::InstantFall:
            return gm.tryInstantFall();
        case Action::UseBag:
            return handleBag(royalGame);

        case Action::None:
            return false;
        default:
            return false;
    }
}

void
ClassicEngine::handleBasicPenalty(ClassicGame &game, const int linesCleared) {
    // this should handle the line penalty for the opponents
    // the penalty is calculated based on the number of lines cleared

    if (linesCleared <= 1) {
        // no penalty lines to add for 0 or 1 lines cleared
        return;
    }

    // get the target
    ClassicGame *opponent = static_cast<ClassicGame *>(game.getTarget());

    if (opponent == nullptr) {
        // no opponent to add penalty lines to
        return;
    }

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

void
ClassicEngine::handleGameLogic(TetrisGame &game) {
    // this should handle the game logic for the classic game mode
    // the game logic should handle the following:
    // - clear the full lines
    // - handle the score
    // - handle the penalty lines for the opponents
    // - handle the spawn of the next piece if needed
    // - check if the game is over

    ClassicGame &classicGame = static_cast<ClassicGame &>(game);

    // clear the full lines and handle the score
    const int linesCleared = classicGame.getGameMatrix().clearFullLines();
    handleScore(classicGame, linesCleared);

    // handle the penalty lines for the opponents
    handleBasicPenalty(classicGame, linesCleared);

    // handle the spawn of the next piece if needed
    handleSpawn(classicGame);

    // check if the game is over
    if (classicGame.isGameOver()) {
        handleGameOver(classicGame);
    }
}

bool
ClassicEngine::viewPreviousOpponent(TetrisGame &game) {
    // this should handle the view of the previous opponent
    // the view should be changed to the previous opponent in the list of
    // opponents

    ClassicGame &classicGame = static_cast<ClassicGame &>(game);

    // get the index of the previous opponent
    int idx = classicGame.getTargetIndex() - 1;

    // change the player view to the previous opponent
    classicGame.changePlayerView(idx);

    return true;
}

bool
ClassicEngine::viewNextOpponent(TetrisGame &game) {
    // this should handle the view of the next opponent
    // the view should be changed to the next opponent in the list of opponents

    ClassicGame &classicGame = static_cast<ClassicGame &>(game);

    // get the index of the next opponent
    int idx = classicGame.getTargetIndex() + 1;

    // change the player view to the next opponent
    classicGame.changePlayerView(idx);

    return true;
}
