#include "TetrisFactory.hpp"


TetrisFactory::TetrisFactory() : rng(rd()) {

    // this is the constructor of the TetrisFactory class
    // it initializes the random number generator with a random device
    // and fills the pool with pieces

    fillPool();

}

TetrisFactory::~TetrisFactory() {

    // this is the destructor of the TetrisFactory class
    // it does nothing special, but it's here for completeness
    // in case we need to clean up resources in the future

}



void TetrisFactory::pushPiece(const Tetromino &tetromino) {
    pool.push_back(tetromino);
}

Tetromino TetrisFactory::popPiece() {
    
    // this function pops a piece from the pool
    // if the pool is empty, it fills it with new pieces
    // and then pops a piece from the back of the pool

    if (pool.empty()) { fillPool(); }

    Tetromino piece = pool.back();
    pool.pop_back();

    return piece;

}

Tetromino &TetrisFactory::whatIsNextPiece() {
    
    // this function returns a reference to the next piece in the pool
    // if the pool is empty, it fills it with new pieces
    // and then returns a reference to the back of the pool

    if (pool.empty()) {
        fillPool();
    }

    return pool.back();

}

bool TetrisFactory::isPoolEmpty() const {
    return pool.empty();
}

int TetrisFactory::getPoolSize() const {
    return static_cast<int>(pool.size());
}

void TetrisFactory::fillPool() {
    
    // this function fills the pool with new pieces
    // if the pool is empty, it creates a new vector of possible pieces
    // and shuffles it using the random number generator

    if (pool.empty()) {

        pieceVec newPool = POSSIBLE_PIECES;
        std::ranges::shuffle(newPool, rng);

        for (const PieceType &piece_t : newPool) {
            Tetromino tetromino = Tetromino(piece_t);
            pushPiece(tetromino);
        }
    }

}
