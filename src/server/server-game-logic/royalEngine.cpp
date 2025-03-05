#include "royalEngine.hpp"


bool RoyalEngine::handleAction(RoyalGame &game, const Action action) {
    
    bool success = true;
    auto& gm = game.getGameMatrix();

    switch (action) {

        case MoveLeft:
            success = gm.tryMoveCurrent(-1, 0);
            break;

        case MoveRight:
            success = gm.tryMoveCurrent(1, 0);
            break;

        case MoveDown:
            success = gm.tryMoveCurrent(0, 1);
            break;

        case RotateLeft:
            success = gm.tryRotateCurrent(false);
            break;

        case RotateRight:
            success = gm.tryRotateCurrent(true);
            break;

        case InstantFall:
            // Instant fall: keep moving down until you can't
            while (gm.tryMoveCurrent(0, 1)) {}
            success = true;
            break;

        case UseBag:
            success = handleBag(game);
            break;

        case Malus:
            
        // !! move logic in handleMalus

            if (game.getEnergy() >= 100) {
                game.setEnergy(0);
                success = handleMalus(game);
            } else {
                success = false;
            }

            break;
            
        case Bonus:

        // !! move logic in handleBonus

            if (game.getEnergy() >= 100) {
                game.setEnergy(0);
                success = handleBonus(game);
            } else {
                success = false;
            }

            break;

        // !! I don't understand
        case None:               
        default:
            // No action
            break;
    }

    return success;

}

bool RoyalEngine::handlePlacingPiece(RoyalGame &game) {
    
    auto& gm = game.getGameMatrix();
    const Tetromino* current = gm.getCurrent();

    // if no current piece then do nothing
    if (!current) return false;

    // if the current piece is still falling, do nothing
    if (const int rowsToObstacle = gm.getRowsToObstacle(*current); rowsToObstacle > 0) return false;

    // reached : place the piece
    const bool placed = gm.tryPlaceCurrentPiece();

    if (placed) {

        // After placing a piece, the bag becomes usable again
        game.getBag().setUsable(true);

        // !! this need to use a boolean flag
        if (game.getBlockFlag()) {
            game.blockControls();
        }

        if (game.getReverseFlag()) {

            // !! also 3 is a magic number, should be a constant
            game.setReversePieceCount(1);

            if (game.getReverseControlTimeCount() == 3) {

                game.setInvertedFlag(true);
                game.setReversePieceCount(-3);
                game.setReverseFlag(false);

            }

        }

    }

    // return the place flag (if false then we will invoke some gameover / respawn logic)
    return placed;

}

void RoyalEngine::handleGameLogic(RoyalGame &game) {

    const int linesCleared = game.getGameMatrix().clearFullLines();
    handleScore(game, linesCleared);
    handleSpawn(game);
    handleEnergy(game, linesCleared);

    if (game.isGameOver()) handleGameOver(game);

}

void RoyalEngine::handlingRoutine(RoyalGame &game, const Action action) {
    

    handleAction(game, game.getBlockFlag() ? None : action);

    // ?? dark mode logic, need to be checked later
    if (game.getDarkMode() && game.getDarkModeTimer() == game.getFrameCount()) {
        game.setDarkMode(false);
        game.setDarkModeTimer(0);
    }

    if (!handleFallingPiece(game)) { handlePlacingPiece(game); }
    handleGameLogic(game);
    game.incrementFrameCount();

}

bool RoyalEngine::handleBonus(RoyalGame &game)  {
    // power ups, self inflicted

    switch (bonusVector[rand() % bonusVector.size()]){

        // fill the pool with 1x1 blocs
        case singleBlocks:
            game.pushSingleBlock();
            break;

        // makes the pieces fall slower
        case slowPieces:
            game.slowPieces();
            break;
        
        default:
            std::cerr << "Unexpected Power up";
            return false;
            break;
    }

    return true;

}

bool RoyalEngine::handleMalus(RoyalGame &game)  {
    switch (malusVector[rand() % malusVector.size()]){

        case invertedControls:
            game.setInvertedFlag(true);
            game.setReverseFlag(true);
            break;

        case blockControls:
            game.blockControls();
            break;

        case thunderStrike:
            game.thunderStrike();
            break;

        case fastPieces:
            game.fastPieces();
            break;

        case darkMode:
            game.darkMode();
            break;

        default:
            std::cerr << "Unexpected Power up";
            return false;
            break;
    }

    return true;

}

