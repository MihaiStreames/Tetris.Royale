#include "tetrisFactory.hpp"

void TetrisFactory::fillPool() {
    if (pool.empty()) {
        pieceVec newPool = possiblePieces;
        std::ranges::shuffle(newPool, rng);

        pool.clear();
        for (auto p : newPool) {
            pool.emplace_back(Position2D{0,0}, p);
        }
    }
}

void TetrisFactory::pushPiece(const Tetromino& tetromino) { pool.push_back(tetromino); }

Tetromino TetrisFactory::popPiece() {
    if (pool.empty()) fillPool();

    Tetromino piece = pool.back();
    pool.pop_back();

    return piece;
}

[[nodiscard]] Tetromino& TetrisFactory::whatIsNextPiece() {
    if (pool.empty()) fillPool();
    return pool.back();
}

[[nodiscard]] bool TetrisFactory::isPoolEmpty() const  { return pool.empty(); }
[[nodiscard]] int TetrisFactory::getPoolSize() const   { return static_cast<int>(pool.size()); }