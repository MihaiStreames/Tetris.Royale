#pragma once

#include <vector>
#include "common.hpp"
#include "tetromino.hpp"

class TetrisFactory {
    std::vector<PieceType> possiblePieces = {I,O,T,S,Z,J,L};
    std::vector<Tetromino> pool;

public:
    void setPossiblePieces(const std::vector<PieceType>& newPossiblePieces) { possiblePieces = newPossiblePieces; }
    std::vector<PieceType> getPossiblePieces() const { return possiblePieces; }

    void setPool(const std::vector<Tetromino>& newPool) { pool = newPool; }
    std::vector<Tetromino> getPool() const { return pool; }

    void fillPool();
    void pushPiece(const Tetromino& tetromino) { pool.push_back(tetromino); }
    Tetromino popPiece();
    Tetromino whatIsNextPiece() const;
    bool isPoolEmpty() const { return pool.empty(); }
    int getPoolSize() const { return static_cast<int>(pool.size()); }
};