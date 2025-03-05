#include "royalEngine.hpp"


bool RoyalEngine::handleAction(TetrisGame& game, const Action action) {
    
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

bool RoyalEngine::handlePlacingPiece(TetrisGame& game) {
    
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
        if (game.getBlockCommand()) {
            game.block_command();
        }

        if (game.getActiveReverseControl()) {

            // !! what is this? what's the timer goal?
            // !! also 3 is a magic number, should be a constant
            // !! the names should also be more descriptive (set -> increment and timecount != timer)
            game.setReverseControlTimeCount(1);

            if (game.getReverseControlTimeCount() == 3) {

                game.inverted_command(true);
                game.setReverseControlTimeCount(-3);
                game.setActiveReverseControl(false);

            }

        }

    }

    // return the place flag (if false then we will invoke some gameover / respawn logic)
    return placed;

}

void RoyalEngine::handleGameLogic(TetrisGame& game) {

    const int linesCleared = game.getGameMatrix().clearFullLines();
    handleScore(game, linesCleared);
    handleSpawn(game);
    handleEnergy(game, linesCleared);

    if (game.isGameOver()) handleGameOver(game);

}

void RoyalEngine::handleEnergy(TetrisGame &game, const int linesCleared) {
    game.calculateEnergy(linesCleared);
}

void RoyalEngine::handlingRoutine(TetrisGame& game, const Action action) {
    

    handleAction(game, game.getBlockCommand() ? None : action);

    // ?? dark mode logic, need to be checked later
    if (game.getDarkMode() && game.getDarkModeTimer() == game.getFrameCount()) {
        game.setDarkMode(false);
        game.setDarkModeTimer(0);
    }

    if (!handleFallingPiece(game)) { handlePlacingPiece(game); }
    handleGameLogic(game);
    game.incrementFrameCount();

}

bool RoyalEngine::handleBonus(TetrisGame &game)  {

    TypePowerUps randomBonus = bonusVector[rand() % bonusVector.size()];

    // power ups, self inflicted

    switch (randomBonus){

        // fill the pool with 1x1 blocs
        case blocs_1x1:
            game.blocs_1x1();
            break;

        // makes the pieces fall slower
        case slow_falling_pieces:
            game.slow_falling_pieces();
            break;
        
        default:
            std::cerr << "Unexpected Power up";
            return false;
            break;
    }

    return true;

}

bool RoyalEngine::handleMalus(TetrisGame &game)  {

    TypePowerUps randomMalus = malusVector[rand() % malusVector.size()];

    switch (randomMalus){

        case inverted_command:
            game.inverted_command(true);
            game.setActiveReverseControl(true);
            break;

        case block_command:
            game.block_command();
            break;

        case thunder_strike:
            game.thunder_strike();
            break;

        case fast_falling_pieces:
            game.fast_falling_pieces();
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

