#pragma once

#include <iostream>
#include <vector>
#include "common.cpp"
#include "tetromino.cpp"

using namespace std;

class TetrisFactory {
    vector<PieceType> possiblePieces = {
        PieceType.I,
        PieceType.O,
        PieceType.T,
        PieceType.S,
        PieceType.Z,
        PieceType.J,
        .L
}

    vector<Tetromino> pool;

public:
    void setPossiblePieces(vector<PlaceType> newPossiblePieces) { possiblePieces = newPossiblePieces; }
    vector<PlaceType> getPossiblePieces() { return possiblePieces; }

    void setPool(vector<Tetromino> newPool) { pool = newPool; }
    vector<Tetromino> getPool() { return pool; }

    void fillPool();
    void pushPiece(Tetromino tetromino);
    Tetromino popPiece();
    Tetromino whatIsNextPiece();
    bool isPoolEmpty();
    int getPoolSize();
};