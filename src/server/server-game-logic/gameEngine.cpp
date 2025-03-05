#include "gameEngine.hpp"


bool GameEngine::handleAction(RoyalGame &game, const Action action) {

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
            // ?? do that in GAMEMATRIX ?
            // Instant fall: keep moving down until you can't
            while (gm.tryMoveCurrent(0, 1)) {}
            success = true;
            break;

        case UseBag:
            success = handleBag(game);
            break;

        // if none of the above, do nothing
        case None:
            break;

        default:
            break;
            
    }

    return success;
}


bool GameEngine::handleBag(TetrisGame& game) {

    auto& bag = game.getBag();
    auto& gm = game.getGameMatrix();

    if (!bag.usable()) { return false; }

    const Tetromino* current = gm.getCurrent();
    if (!current) return false; // no current piece to store or swap

    if (bag.isEmpty()) {

        bag.storePiece(*current);
        gm.deleteCurrent();
        handleSpawn(game); // spawn a new piece from the factory

    } else {

        // Swap with the stored piece
        // Put current piece back into the factory pool
        Tetromino resetCurrent = *current;
        resetCurrent.reset();

        // Push the reset tetromino back into the factory pool
        game.getFactory().pushPiece(resetCurrent);
        gm.deleteCurrent();
        Tetromino retrieved = bag.retrievePiece();
        handleSpawn(game, retrieved);

    }

    bag.setUsable(false);
    return true;

}

bool GameEngine::handleFallingPiece(TetrisGame& game) {
    if (!game.shouldApplyGravity()) return true;
    return game.getGameMatrix().tryMakeCurrentPieceFall();
}

bool GameEngine::handlePlacingPiece(RoyalGame &game) {
    
    auto& gm = game.getGameMatrix();
    const Tetromino* current = gm.getCurrent();
    if (!current) return false;

    if (const int rowsToObstacle = gm.getRowsToObstacle(*current); rowsToObstacle > 0) return false; // still space before placing

    const bool placed = gm.tryPlaceCurrentPiece();
    if (placed) {
        // After placing a piece, the bag becomes usable again
        game.getBag().setUsable(true);
    }

    return placed;

}

void GameEngine::handleSpawn(TetrisGame& game) {

    if (auto& gm = game.getGameMatrix(); !gm.getCurrent()) {

        const Tetromino piece = game.getFactory().popPiece();
        if (const bool success = gm.trySpawnPiece(piece); !success) {
            handleGameOver(game);
        }

    }

}

void GameEngine::handleSpawn(TetrisGame &game, Tetromino &piece) {

    if (auto& gm = game.getGameMatrix(); !gm.getCurrent()) {

        if (const bool success = gm.trySpawnPiece(piece); !success) {
            // If we failed to spawn, the game is over
            game.setGameOver(true);
            handleGameOver(game);
        }

    }

}

void GameEngine::handleGameLogic(RoyalGame &game) {

    const int linesCleared = game.getGameMatrix().clearFullLines();
    handleScore(game, linesCleared);
    handleSpawn(game);  // ensure a piece is always available

    if (game.isGameOver()) handleGameOver(game);

}

void GameEngine::handleGameOver(TetrisGame& game /*eventually have game*/) {
    (void) game;
    std::cerr << "Game Over!" << std::endl;
    exit(0); // !! later
}

void GameEngine::handleScore(TetrisGame& game, const int linesCleared) {
    
    game.incrementLinesCleared(linesCleared);
    game.calculateScore(linesCleared);

    if (game.shouldLevelUp()) game.updateLevelAfterLineClear();

}

void GameEngine::handlingRoutine(RoyalGame &game, const Action action) {

    handleAction(game, action);

    if (!handleFallingPiece(game)) {
        handlePlacingPiece(game);
    }

    handleGameLogic(game);

    game.incrementFrameCount();

}
