#include <random>
#include <algorithm>
#include <stdexcept>
#include "tetrisFactory.hpp"

void TetrisFactory::fillPool() {
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

Tetromino TetrisFactory::popPiece() {
    if (pool.empty()) throw std::runtime_error("Pool is empty");

    Tetromino piece = pool.back();
    pool.pop_back();
    return piece;
}

Tetromino TetrisFactory::whatIsNextPiece() const {
    if (pool.empty()) throw std::runtime_error("Pool is empty");

    return pool.back();
}