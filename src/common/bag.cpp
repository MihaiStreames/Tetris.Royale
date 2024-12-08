#include <stdexcept>
#include "bag.hpp"

Bag::Bag() : storedPiece(nullptr), isUsable(true) {}

void Bag::storePiece(const Tetromino& piece) {
    if (isUsable && isEmpty()) {
        storedPiece = new Tetromino(piece);
    }
}

Tetromino Bag::retrievePiece() {
    if (!isEmpty()) {
        Tetromino piece = *storedPiece;
        delete storedPiece;
        storedPiece = nullptr;
        return piece;
    }

    throw std::runtime_error("Bag is empty.");
}

Bag::~Bag() {
    delete storedPiece;
}