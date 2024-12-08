#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include "types.hpp"
#include "tetromino.hpp"

using tetroVec = std::vector<Tetromino>;

class TetrisFactory {
    const pieceVec possiblePieces = { I, O, T, S, Z, J, L };
    tetroVec pool;

    std::random_device rd;
    std::mt19937 rng;

public:
    TetrisFactory() : rng(rd()) { fillPool(); }

    void fillPool() {
        if (pool.empty()) {
            pieceVec newPool = possiblePieces;
            std::ranges::shuffle(newPool, rng);

            pool.clear();
            for (auto p : newPool) {
                pool.emplace_back(Position2D{0,0}, p);
            }
        }
    }

    void pushPiece(const Tetromino& tetromino) { pool.push_back(tetromino); }

    Tetromino popPiece() {
        if (pool.empty()) fillPool();

        Tetromino piece = pool.back();
        pool.pop_back();

        return piece;
    }

    [[nodiscard]] Tetromino& whatIsNextPiece() {
        if (pool.empty()) fillPool();
        return pool.back();
    }

    [[nodiscard]] bool isPoolEmpty() const  { return pool.empty(); }
    [[nodiscard]] int getPoolSize() const   { return static_cast<int>(pool.size()); }
};