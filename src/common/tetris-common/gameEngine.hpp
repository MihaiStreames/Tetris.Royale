#pragma once

#include <iostream>
#include "tetrisGame.hpp"

class GameEngine {
public:
    static bool handleAction(TetrisGame& game, const Action action) {
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
            case None:
            default:
                // No action
                break;
        }

        return success;
    }

    static bool handleBag(TetrisGame& game) {
        auto& bag = game.getBag();
        auto& gm = game.getGameMatrix();

        if (!bag.usable()) return false;

        const Tetromino* current = gm.getCurrent();
        if (!current) return false; // no current piece to store or swap

        if (bag.isEmpty()) {
            bag.storePiece(*current);
            gm.deleteCurrent();
            handleSpawn(game); // spawn a new piece from the factory
        } else {
            // Swap with the stored piece
            // Put current piece back into the factory pool
            game.getFactory().pushPiece(*current);
            gm.deleteCurrent();
            Tetromino retrieved = bag.retrievePiece();
            handleSpawn(game, retrieved);
        }

        bag.setUsable(false);
        return true;
    }

    static bool handleFallingPiece(TetrisGame& game) {
        if (!game.shouldApplyGravity()) return true;
        return game.getGameMatrix().tryMakeCurrentPieceFall();
    }

    static bool handlePlacingPiece(TetrisGame& game) {
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

    static void handleSpawn(TetrisGame& game) {
        if (auto& gm = game.getGameMatrix(); !gm.getCurrent()) {
            const Tetromino piece = game.getFactory().popPiece();
            if (const bool success = gm.trySpawnPiece(piece); !success) {
                handleGameOver(game);
            }
        }
    }

    static void handleSpawn(TetrisGame &game, Tetromino &piece) {
        if (auto& gm = game.getGameMatrix(); !gm.getCurrent()) {
            if (const bool success = gm.trySpawnPiece(piece); !success) {
                // If we failed to spawn, the game is over
                game.setGameOver(true);
                handleGameOver(game);
            }
        }
    }

    static void handleGameLogic(TetrisGame& game) {
        const int linesCleared = game.getGameMatrix().clearFullLines();
        handleScore(game, linesCleared);
        handleSpawn(game);  // ensure a piece is always available

        if (game.isGameOver()) handleGameOver(game);
    }

    static void handleGameOver(TetrisGame& /*eventually have game*/) {
        std::cerr << "Game Over!" << std::endl;
        exit(0); // TODO: later
    }

     static void handleScore(TetrisGame& game, const int linesCleared) {
        game.incrementLinesCleared(linesCleared);
        game.calculateScore(linesCleared);

        if (game.shouldLevelUp()) game.updateLevelAfterLineClear();
    }

    // The action should be passed from outside
    static void handlingRoutine(TetrisGame& game, const Action action) {
        handleAction(game, action);

        if (!handleFallingPiece(game)) {
            handlePlacingPiece(game);
        }

        handleGameLogic(game);

        game.incrementFrameCount();
    }
};