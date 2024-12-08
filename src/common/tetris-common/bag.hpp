#pragma once

#include <stdexcept>

#include "tetromino.hpp"

class Bag {
    Tetromino* storedPiece; // Why is this not a smart pointer
    bool isUsable;

public:
    Bag() : storedPiece(nullptr), isUsable(true) { }
    ~Bag() { delete storedPiece; }

    Tetromino retrievePiece() {
        if (!isEmpty()) {
            Tetromino piece = *storedPiece;
            delete storedPiece;
            storedPiece = nullptr;
            return piece;
        }

        throw std::runtime_error("Bag is empty.");
    }

    [[nodiscard]] bool isEmpty() const { return storedPiece == nullptr; }
    void storePiece(const Tetromino& piece){
        if (isUsable && isEmpty()) {
            storedPiece = new Tetromino(piece);
        }
    }
};