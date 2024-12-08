#pragma once

#include <vector>
#include "common.hpp"
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

    void fillPool();
    void pushPiece(const Tetromino& tetromino) { pool.push_back(tetromino); }
    Tetromino popPiece();
    [[nodiscard]] Tetromino whatIsNextPiece() const;
    [[nodiscard]] bool isPoolEmpty() const { return pool.empty(); }
    [[nodiscard]] int getPoolSize() const { return static_cast<int>(pool.size()); }
};