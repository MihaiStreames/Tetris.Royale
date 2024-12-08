#pragma once

#include "tetromino.hpp"

class Bag {
    Tetromino* storedPiece;
    bool isUsable;

public:
    Bag();
    ~Bag();

    Tetromino retrievePiece();
    [[nodiscard]] bool isEmpty() const { return storedPiece == nullptr; }
    void storePiece(const Tetromino& piece);
};