#pragma once

#include "tetromino.hpp"

class Bag {
    Tetromino* storedPiece;
    bool isUsable;

public:
    Bag();
    ~Bag();
    Tetromino retrievePiece();
    bool isEmpty() const { return storedPiece == nullptr; }
    void storePiece(const Tetromino& piece);
};