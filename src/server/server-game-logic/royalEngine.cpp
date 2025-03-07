
#include "royalEngine.hpp"



bool RoyalEngine::handleAction(TetrisGame &game, const Action action) {
    
    // this method is responsible for handling the game logic
    // based on the action that the player has taken

    // will throw an exception if the action is invalid
    // handled actions include : classic moves, special moves, malus, bonus and none

    RoyalGame &royalGame = static_cast<RoyalGame&>(game);
    GameMatrix& gm = royalGame.getGameMatrix();

    switch (action) {

        // classic moves
        case MoveLeft: return gm.tryMoveLeft();
        case MoveRight: return gm.tryMoveRight();
        case MoveDown: return gm.tryMoveDown();
        case RotateLeft: return gm.tryRotateLeft();
        case RotateRight: return gm.tryRotateRight();

        // special moves
        case InstantFall: return gm.tryInstantFall();
        case UseBag: return handleBag(royalGame);

        // power ups
        case UseMalus: handleMalus(royalGame); return true;
        case UseBonus: handleBonus(royalGame); return true;

        case None: return false;
        default: throw std::runtime_error("[err] Invalid action: " + static_cast<int>(action));

    }

}

void RoyalEngine::handleGameLogic(TetrisGame &game) {

    // this method is responsible for handling the game logic
    // based on the current state of the game

    RoyalGame &royalGame = static_cast<RoyalGame&>(game);

    const int linesCleared = royalGame.getGameMatrix().clearFullLines();
    handleScore(royalGame, linesCleared);
    handleSpawn(royalGame);

    handleEnergy(royalGame, linesCleared);

    if (royalGame.isGameOver()) { handleGameOver(royalGame); }

}



void RoyalEngine::handleEnergy(TetrisGame &game, const int linesCleared) {

    // this method is responsible for handling the energy
    // based on the number of lines cleared

    // TODO : maybe increment energy as time goes on (but that's a stretch)

    RoyalGame &royalGame = static_cast<RoyalGame&>(game);

    if (linesCleared > 0) {
        royalGame.updateEnergy(linesCleared);
    }

}

bool RoyalEngine::hasEnoughEnergy(RoyalGame &game) {

    // this method is responsible for checking if the player has enough energy
    // to use a special move

    return game.getEnergy() >= POWER_UP_COST;

}

void RoyalEngine::handlingRoutine(TetrisGame &game, const Action action) {

    // this method is responsible for handling the game logic
    // based on the current state of the game

    RoyalGame &royalGame = static_cast<RoyalGame&>(game);
    
    // call the handleAction method with the given action if the block flag is not set,
    // otherwise call the handleAction method with the None action -> blocking effect
    (void) handleAction(royalGame, royalGame.getBlockControlsFlag() ? None : action);

    // if darkmode flag is enabled, will check if the current frame is the same as the frame when the darkmode was enabled
    // + the duration of the darkmode, if so, will disable the darkmode
    if (royalGame.getDarkModeFlag() && royalGame.getDarkModeTimer() == royalGame.getFrameCount()) {
        royalGame.setDarkModeFlag(false);
        royalGame.setDarkModeTimer(-1);
    }

    // if the piece's is not falling, then try to place the piece
    if (!handleFallingPiece(royalGame)) {

        bool couldPlace = handlePlacingPiece(royalGame);

        // if we were able to place, then we need to update the malus flags
        if (couldPlace) { handleGameFlags(royalGame); }

    }

    // game logic + fc
    handleGameLogic(game);
    game.incrementFrameCount(1);

}

void RoyalEngine::handleGameFlags(RoyalGame &game) {

    // this method is responsible for handling the game flags
    // based on the current state of the game

    // if the block flag is set, then we need to set the block command to false
    if (game.getBlockControlsFlag()) { game.setBlockControlsFlag(false); }

    // if the inverted flag is set, then we need to manage its logic
    if (game.getReverseControlsFlag()) {

        // increment the count by 1
        game.incrementMalusCooldown(1);

        // if the malus cooldown is greater than some limit, then we need to reset the inverted flag
        if (game.getMalusCooldown() > INVERTED_CONTROLS_COOLDOWN) {
            game.setReverseControlsFlag(false);
            game.setMalusCooldown(0);
        }

    }

}


void RoyalEngine::handleBonus(TetrisGame &game)  {
    
    // this method is responsible for handling the bonus
    // Note that this is self inflicted, so the player will always
    // target itself with the bonus power up thingy

    RoyalGame &royalGame = static_cast<RoyalGame&>(game);

    // if the player does not have enough energy, then return
    // otherwise, decrement the energy by the cost of the power up and continue
    if (!hasEnoughEnergy(royalGame)) { return; }
    else { royalGame.incrementEnergy(-POWER_UP_COST); }

    // select a random power up from the bonus vector (defined in 'types.hpp')
    TypePowerUps randomBonus = bonusVector[rand() % bonusVector.size()];

    switch (randomBonus) {

        case singleBlocks: return royalGame.pushSingleBlock();
        case slowPieces: return royalGame.decreaseFallingSpeed();
        
        default: throw std::runtime_error("[err] Unexpected Bonus");

    }

}

void RoyalEngine::handleMalus(TetrisGame &game) {

    // this method is responsible for handling the malus
    // note that this is not self inflicted, meaning this will target the opponent
    // of the game given in parameter

    RoyalGame &royalGame = static_cast<RoyalGame&>(game);
    RoyalGame* opponent = static_cast<RoyalGame*>(royalGame.getTarget());

    // funny stuff here : if the opponent is null, then self inflict the malus
    if (opponent == nullptr) { opponent = &royalGame; }

    // if the player does not have enough energy, then return
    // otherwise, decrement the energy by the cost of the power up and continue
    if (!hasEnoughEnergy(royalGame)) { return; }
    else { royalGame.incrementEnergy(-POWER_UP_COST); }

    // select a random power up from the malus vector (defined in 'types.hpp')
    TypePowerUps randomMalus = malusVector[rand() % malusVector.size()];

    switch (randomMalus) {

        case invertedControls: opponent->startInvertedControls(); break;
        case blockControls: opponent->startBlockControls(); break;
        case thunderStrike: opponent->spawnThunderStrike(); break;
        case fastPieces: opponent->increaseFallingSpeed(); break;
        case darkMode: opponent->startDarkMode(); break;

        default: throw std::runtime_error("[err] Unexpected Malus");
        
    }

}

