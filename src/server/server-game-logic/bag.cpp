
#include "bag.hpp"



Bag::Bag() : storedPiece(nullptr), isUsable(true) {

    // constructor for the bag, initializes the stored piece to nullptr and the bag to usable
    // this does nothing else for now

}

Bag::~Bag() {

    // destructor for the bag, does nothing for now

}



bool Bag::isEmpty() const {
    // returns true if the bag is empty, false otherwise
    return storedPiece == nullptr;
}

bool Bag::isBagUsable() const {
    // returns true if the bag is usable, false otherwise
    return isUsable;
}

void Bag::setUsable(const bool flag) {
    // sets the bag to usable or not usable
    isUsable = flag;
}


const Tetromino* Bag::peekPiece() const {

    // this returns the stored piece if it exists, otherwise nullptr
    // this is useful for checking the next piece without removing it from the bag

    if (isEmpty()) { return nullptr; }
    return storedPiece.get();

}


Tetromino Bag::retrievePiece() {

    // this returns the stored piece if it exists, otherwise shouldn't be called
    // if the bag is empty, will throw a runtime error

    if (isEmpty()) { 
        throw std::runtime_error("[err] Tried to retrieve piece, but bag is empty.");
    }

    // we want to return the stored piece, and then reset it (to avoid keeping the state it had when entering the bag)
    Tetromino piece = *storedPiece;
    storedPiece.reset();
    return piece;

}


void Bag::storePiece(const Tetromino& piece) {

    // this stores a piece in the bag, if the bag is usable and empty
    // if the bag is not usable, or not empty, this does nothing

    if (isBagUsable() && isEmpty()) { 
        storedPiece = std::make_unique<Tetromino>(piece); 
        // call a reset on the piece to avoid keeping the state it had when entering the bag
        // (already done in the retrievePiece function, but we do it here as well for consistency)
        storedPiece->reset();
    }

}

