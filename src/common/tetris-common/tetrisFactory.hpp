#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>

#include "types.hpp"
#include "tetromino.hpp"

using tetroVec = std::vector<Tetromino>;

class TetrisFactory {
    pieceVec possiblePieces = {I,O,T,S,Z,J,L};
    tetroVec pool;

public:
    void setPossiblePieces(const pieceVec& newPossiblePieces) { possiblePieces = newPossiblePieces; }
    [[nodiscard]] pieceVec getPossiblePieces() const { return possiblePieces; }

    void setPool(const tetroVec& newPool) { pool = newPool; }
    [[nodiscard]] tetroVec getPool() const { return pool; }

    void fillPool() {
        if (pool.size() == 1) {
            const Tetromino firstPiece = pool[0];
            std::vector newPool = {I,O,T,S,Z,J,L};

            for (int i = 0; i < static_cast<int>(newPool.size()); i++) {
                if (newPool[i] == firstPiece.getPieceType()) {
                    newPool.erase(newPool.begin() + i);
                    break;
                }
            }

            std::random_device rd;
            std::mt19937 rng(rd());
            std::ranges::shuffle(newPool, rng);

            newPool.push_back(firstPiece.getPieceType());
            pool.clear();

            for (auto p : newPool) {
                pool.emplace_back(Position2D{0,0}, p);
            }
        }
    }
    void pushPiece(const Tetromino& tetromino) { pool.push_back(tetromino); }
    Tetromino popPiece() {
        if (pool.empty()) throw std::runtime_error("Pool is empty");

        Tetromino piece = pool.back();
        pool.pop_back();
        return piece;
    }
    [[nodiscard]] Tetromino whatIsNextPiece() const {
        if (pool.empty()) throw std::runtime_error("Pool is empty");
        return pool.back();
    }
    [[nodiscard]] bool isPoolEmpty() const { return pool.empty(); }
    [[nodiscard]] int getPoolSize() const { return static_cast<int>(pool.size()); }
};