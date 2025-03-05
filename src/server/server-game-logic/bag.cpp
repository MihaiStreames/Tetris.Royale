#include "bag.hpp"

Tetromino Bag::retrievePiece() {
    if (!isEmpty()) { 
        Tetromino piece = *storedPiece; // Copy the stored piece.
        storedPiece.reset(); // Clear the stored piece from the bag.
        return piece; 
    }

    throw std::runtime_error("Bag is empty."); // Throw an error if the bag is empty.
}

[[nodiscard]] const Tetromino* Bag::peekPiece() const { return storedPiece.get(); }

[[nodiscard]] bool Bag::isEmpty() const { return storedPiece == nullptr; }

[[nodiscard]] bool Bag::usable() const { return isUsable; }

void Bag::setUsable(const bool flag) { isUsable = flag; }

void Bag::storePiece(const Tetromino& piece) {
    if (isUsable && isEmpty()) { 
        storedPiece = std::make_unique<Tetromino>(piece); 
        storedPiece->reset(); // Reset the Tetromino to its default state.
    }
}