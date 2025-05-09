#include "RoyalEngine.hpp"

bool
RoyalEngine::handleAction(TetrisGame &game, const Action action) {
    // this method is responsible for handling the game logic
    // based on the action that the player has taken

    // will throw an exception if the action is invalid
    // handled actions include : classic moves, special moves, malus, bonus and
    // none

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);
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

        // power ups
        case Action::UseMalus:
            handleMalus(royalGame);
            return true;
        case Action::UseBonus:
            handleBonus(royalGame);
            return true;

        case Action::None:
            return false;
        default:
            return false;
    }
}

bool
RoyalEngine::viewPreviousOpponent(TetrisGame &game) {

    // okay so this oop is disgusting but whatever
    // i need to fix this quick cuz else the project will have crashes
    // when trying to do whatever with the opponents in a royal game

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);

    // get the index of the previous opponent
    int idx = royalGame.getTargetIndex() - 1;

    // change the player view to the previous opponent
    royalGame.changePlayerView(idx);

    return true;
}

bool
RoyalEngine::viewNextOpponent(TetrisGame &game) {

    // this should handle the view of the next opponent
    // the view should be changed to the next opponent in the list of opponents

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);

    // get the index of the next opponent
    int idx = royalGame.getTargetIndex() + 1;

    // change the player view to the next opponent
    royalGame.changePlayerView(idx);

    return true;
}

void
RoyalEngine::handleGameLogic(TetrisGame &game) {
    // this method is responsible for handling the game logic
    // based on the current state of the game

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);

    const int linesCleared = royalGame.getGameMatrix().clearFullLines();
    handleScore(royalGame, linesCleared);
    handleSpawn(royalGame);

    handleEnergy(royalGame, linesCleared);

    if (royalGame.isGameOver()) {
        handleGameOver(royalGame);
    }
}

void
RoyalEngine::handleEnergy(TetrisGame &game, const int linesCleared) {
    // this method is responsible for handling the energy
    // based on the number of lines cleared

    // TODO : maybe increment energy as time goes on (but that's a stretch)

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);

    if (linesCleared > 0) {
        royalGame.updateEnergy(linesCleared);
    }
}

bool
RoyalEngine::hasEnoughEnergy(RoyalGame &game) {
    // this method is responsible for checking if the player has enough energy
    // to use a special move

    return game.getEnergy() >= POWER_UP_COST;
}

void
RoyalEngine::handlingRoutine(TetrisGame &game, const Action action) {
    // this method is responsible for handling the game logic
    // based on the current state of the game

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);

    // call the handleAction method with the given action if the block flag is
    // not set, otherwise call the handleAction method with the None action ->
    // blocking effect
    (void) handleAction(
        royalGame, royalGame.getBlockControlsFlag() ? Action::None : action);

    // if darkmode flag is enabled, will check if the current frame is the same
    // as the frame when the darkmode was enabled
    // + the duration of the darkmode, if so, will disable the darkmode
    if (royalGame.getDarkModeFlag() &&
        royalGame.getDarkModeTimer() == royalGame.getFrameCount()) {
        royalGame.setDarkModeFlag(false);
        royalGame.setDarkModeTimer(-1);
    }

    // if the player is in a game over state, don't do anything
    if (royalGame.isGameOver()) {
        return;
    }

    // if the piece's is not falling, then try to place the piece
    if (!handleFallingPiece(royalGame)) {
        bool couldPlace = handlePlacingPiece(royalGame);

        // if we were able to place, then we need to update the malus flags
        if (couldPlace) {
            handleGameFlags(royalGame);
        }
    }

    handleGameLogic(game);
    game.incrementFrameCount(1);
}

void
RoyalEngine::handleGameFlags(RoyalGame &game) {
    // this method is responsible for handling the game flags
    // based on the current state of the game

    // if the block flag is set, then we need to set the block command to false
    if (game.getBlockControlsFlag()) {
        game.setBlockControlsFlag(false);
    }

    // if the inverted flag is set, then we need to manage its logic
    if (game.getReverseControlsFlag()) {
        // increment the count by 1
        game.incrementMalusCooldown(1);

        // if the malus cooldown is greater than some limit, then we need to
        // reset the inverted flag
        if (game.getMalusCooldown() > INVERTED_CONTROLS_COOLDOWN) {
            game.setReverseControlsFlag(false);
            game.setMalusCooldown(0);
        }
    }
}

void
RoyalEngine::handleBonus(TetrisGame &game) {
    // this method is responsible for handling the bonus
    // Note that this is self inflicted, so the player will always
    // target itself with the bonus power up thingy

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);

    // if the player does not have enough energy, then return
    // otherwise, decrement the energy by the cost of the power up and continue
    if (!hasEnoughEnergy(royalGame)) {
        return;
    } else {
        royalGame.incrementEnergy(-POWER_UP_COST);
    }

    // select a random power up from the bonus vector (defined in 'types.hpp')
    TypePowerUps randomBonus = bonusVector[rand() % bonusVector.size()];

    switch (randomBonus) {
        case TypePowerUps::singleBlocks:
            return royalGame.pushSingleBlock();
        case TypePowerUps::slowPieces:
            return royalGame.decreaseFallingSpeed();

        default:
            throw std::runtime_error("[err] Unexpected Bonus");
    }
}

void
RoyalEngine::handleMalus(TetrisGame &game) {
    // this method is responsible for handling the malus
    // note that this is not self inflicted, meaning this will target the
    // opponent of the game given in parameter

    RoyalGame &royalGame = static_cast<RoyalGame &>(game);
    RoyalGame *opponent = static_cast<RoyalGame *>(royalGame.getTarget());

    // funny stuff here : if the opponent is null, then self inflict the malus
    if (opponent == nullptr) {
        opponent = &royalGame;
    }

    // if the player does not have enough energy, then return
    // otherwise, decrement the energy by the cost of the power up and continue
    if (!hasEnoughEnergy(royalGame)) {
        return;
    } else {
        royalGame.incrementEnergy(-POWER_UP_COST);
    }

    // select a random power up from the malus vector (defined in 'types.hpp')
    TypePowerUps randomMalus = malusVector[rand() % malusVector.size()];

    switch (randomMalus) {
        case TypePowerUps::invertedControls:
            opponent->startInvertedControls();
            break;
        case TypePowerUps::blockControls:
            opponent->startBlockControls();
            break;
        case TypePowerUps::thunderStrike:
            opponent->spawnThunderStrike();
            break;
        case TypePowerUps::fastPieces:
            opponent->increaseFallingSpeed();
            break;
        case TypePowerUps::darkMode:
            opponent->startDarkMode();
            break;

        default:
            throw std::runtime_error("[err] Unexpected Malus");
    }
}
