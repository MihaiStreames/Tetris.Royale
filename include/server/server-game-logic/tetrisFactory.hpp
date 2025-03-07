#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include "types.hpp"
#include "tetromino.hpp"

class TetrisFactory {

    const pieceVec possiblePieces = {
        PieceType::I, PieceType::O, PieceType::L, PieceType::J,
        PieceType::Z, PieceType::S, PieceType::T
    };
    std::vector<Tetromino> pool;

    std::random_device rd;
    std::mt19937 rng;

public:

    TetrisFactory() : rng(rd()) { fillPool(); }

    void fillPool();
    void pushPiece(const Tetromino& tetromino);
    Tetromino popPiece();

    [[nodiscard]] Tetromino& whatIsNextPiece();
    [[nodiscard]] bool isPoolEmpty() const;
    [[nodiscard]] int getPoolSize() const;

};

