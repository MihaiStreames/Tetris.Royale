#include "bag.hpp"

Tetromino Bag::retrievePiece() {
    if (!isEmpty()) { 
        Tetromino piece = *storedPiece;
        storedPiece.reset();
        return piece; 
    }

    throw std::runtime_error("Bag is empty.");
}

void Bag::storePiece(const Tetromino& piece) {
    if (isUsable && isEmpty()) { 
        storedPiece = std::make_unique<Tetromino>(piece); 
        storedPiece->reset();
    }
}