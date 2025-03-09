
#include "gameEngine.hpp"



bool GameEngine::handleAction(TetrisGame &game, const Action action) {

    // this method is responsible for handling the game logic
    // based on the action that the player has taken

    // will throw an exception if the action is invalid
    // handled actions include : classic moves, special moves, and none

    GameMatrix& gm = game.getGameMatrix();

    switch (action) {
        
        // classic moves
        case Action::MoveLeft: return gm.tryMoveLeft();
        case Action::MoveRight: return gm.tryMoveRight();
        case Action::MoveDown: return gm.tryMoveDown();
        case Action::RotateLeft: return gm.tryRotateLeft();
        case Action::RotateRight: return gm.tryRotateRight();

        // special moves
        case Action::InstantFall: return gm.tryInstantFall();
        case Action::UseBag: return handleBag(game);
        case Action::SeePreviousOpponent: return viewPreviousOpponent(game);
        case Action::SeeNextOpponent: return viewNextOpponent(game);
        case Action::None: return false;

        default: throw std::runtime_error("[err] Invalid action: " + static_cast<int>(action));
            
    }

}



bool GameEngine::handleBag(TetrisGame& game) {

    // this method is responsible for handling the bag
    // the bag is a special feature that allows the player to store a piece
    // and swap it with the current piece

    // will return false if the bag is not usable
    // will return false if there is no current piece to store or swap
    // will return true if the bag is used successfully

    Bag& bag = game.getBag();
    GameMatrix& gm = game.getGameMatrix();

    if (!bag.isBagUsable()) { return false; }

    const Tetromino* current = gm.getCurrent();
    if (!current) {return false; } // no current piece to store or swap


    if (bag.isEmpty()) {
        handleEmptyBag(game);
    } else {
        handleSwap(game);
    }

    return true;

}

void GameEngine::handleEmptyBag(TetrisGame& game) {

    // this method is responsible for handling the bag
    // when the bag is empty, usage will store the current piece
    // and spawn a new piece from the factory

    Bag& bag = game.getBag();
    GameMatrix& gm = game.getGameMatrix();

    bag.storePiece(*gm.getCurrent());
    gm.deleteCurrent();
    handleSpawn(game);

    // the bag is no longer usable after storing a piece
    bag.setUsable(false);

}

void GameEngine::handleSwap(TetrisGame& game) {

    // this method is responsible for handling the bag
    // when the bag is not empty, usage will swap the current piece
    // with the piece stored in the bag

    Bag& bag = game.getBag();
    GameMatrix& gm = game.getGameMatrix();
    Tetromino current = *gm.getCurrent();

    // put the tetro in the factory (top) and removing it from the game matrix
    current.reset();
    game.getFactory().pushPiece(current);
    gm.deleteCurrent();

    // retrieve the tetro from the bag and put it in the game matrix
    Tetromino retrieved = bag.retrievePiece();
    handleSpawn(game, retrieved);

    // the bag is no longer usable after a swap
    bag.setUsable(false);

}



bool GameEngine::handleFallingPiece(TetrisGame& game) {

    // this method is responsible for handling the falling piece
    // the falling piece is the current piece that is being controlled by the player
    // the piece will fall down until it reaches the bottom of the game matrix
    // or until it collides with another piece

    // will return true if the piece doesn't need to fall (not on this frame)
    // will return true if the piece is still falling
    // will return false if the piece has reached the bottom or collided with another piece

    return (!game.shouldApplyGravity()) ? true : game.getGameMatrix().tryMakeCurrentPieceFall();

}

bool GameEngine::handlePlacingPiece(TetrisGame &game) {

    // this method is responsible for handling the placing of the piece
    // the placing of the piece is the final step of the game logic
    // the piece will be placed in the game matrix and a new piece will be spawned

    GameMatrix& gm = game.getGameMatrix();
    const Tetromino* current = gm.getCurrent();

    // check if there is a current piece
    if (!current) { return false; }

    // check if there is space to place the piece
    const int rowsToObstacle = gm.getRowsToObstacle(*current);
    
    if (rowsToObstacle > 0) {
        return false;
    }

    // if reached here, the piece need to be placed
    // if the piece is placed, the bag is usable again
    const bool placed = gm.tryPlaceCurrentPiece();
    if (placed) { game.getBag().setUsable(true); }

    return placed;

}




void GameEngine::handleSpawn(TetrisGame& game) {

    // this method is responsible for handling the spawning of the piece
    // the spawning of the piece is the first step of the game logic
    // a new piece will be spawned from the factory and placed in the game matrix

    GameMatrix& gm = game.getGameMatrix(); 

    // if empty current piece, spawn a new one!
    // the piece to spawn will be popped from the factory
    // if the placing can't be done, the game is over (no space to spawn)

    if (!gm.getCurrent()) {

        const Tetromino piece = game.getFactory().popPiece();
        const bool success = gm.trySpawnPiece(piece);
        
        if (!success) { game.setGameOver(true); }

    }

}

void GameEngine::handleSpawn(TetrisGame &game, Tetromino &piece) {

    // specific version of the handleSpawn method, that allows to spawn a specific piece
    // given in parameter, instead of popping a piece from the factory

    // ?? there's probably a clean way to do this without duplicating the code
    // ?? but time constraints are a thing so I'll leave it like this for now (feel free to edit)

    GameMatrix& gm = game.getGameMatrix(); 

    // if empty current piece, spawn a new one!
    // if the placing can't be done, the game is over (no space to spawn)

    if (!gm.getCurrent()) {

        const bool success = gm.trySpawnPiece(piece);
        if (!success) { game.setGameOver(true); }

    }

}




void GameEngine::handleGameLogic(TetrisGame &game) {

    // this method is responsible for handling the game logic
    // the game logic includes clearing full lines, updating the score, and checking for game over

    const int linesCleared = game.getGameMatrix().clearFullLines();
    handleScore(game, linesCleared);
    handleSpawn(game);

    if (game.isGameOver()) { handleGameOver(game); }

}

void GameEngine::handleGameOver(TetrisGame& game) {

    // this method is responsible for handling the game over
    // TODO : maybe need some stuff like free pointers or some

    (void) game;

}

void GameEngine::handleScore(TetrisGame& game, const int linesCleared) {

    // this method is responsible for handling the score
    // the score is updated based on the number of lines cleared
    // the score is also used to determine if the player should level up
    
    game.incrementLinesCleared(linesCleared);
    game.updateScore(linesCleared);

    if (game.shouldLevelUp()) { game.updateLevelAfterLineClear(); }

}

void GameEngine::handlingRoutine(TetrisGame &game, const Action action) {

    // this method is responsible for handling the game routine
    // the game routine includes handling the action, the falling piece, the placing piece
    // and the game logic

    (void) handleAction(game, action);
    // if the piece could not fall, try to place it
    if (!handleFallingPiece(game)) { (void) handlePlacingPiece(game); }

    handleGameLogic(game);
    game.incrementFrameCount(1);

}



bool GameEngine::viewPreviousOpponent(TetrisGame &game) {

    // !! this method should be implemented in other engines

    (void) game;
    throw std::runtime_error("[err] viewPreviousOpponent not implemented");

}

bool GameEngine::viewNextOpponent(TetrisGame &game) {

    // !! this method should be implemented in other engines

    (void) game;
    throw std::runtime_error("[err] viewNextOpponent not implemented");

}

void GameEngine::handleBonus(TetrisGame &game) {

    // !! this method should be implemented in other engines

    (void) game;
    throw std::runtime_error("[err] handleBonus not implemented");

}

void GameEngine::handleMalus(TetrisGame &game) {

    // !! this method should be implemented in other engines

    (void) game;
    throw std::runtime_error("[err] handleMalus not implemented");

}

void GameEngine::sendToEnemy() {

    // !! this method should be implemented in other engines

    throw std::runtime_error("[err] sendToEnemy not implemented");

}

void GameEngine::handleEnergy() {

    // !! this method should be implemented in other engines

    throw std::runtime_error("[err] handleEnergy not implemented");

}

