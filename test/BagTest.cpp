#include <gtest/gtest.h>

#include "Bag.hpp"
#include "Tetromino.hpp"


TEST(BagTest, Constructor) {
    Bag bag = Bag();
    EXPECT_TRUE(bag.isBagUsable()) << "Bag should be usable after construction.";
    EXPECT_TRUE(bag.isEmpty()) << "Bag should be empty after construction.";
}

TEST(BagTest, Usable) {
    Bag bag = Bag();
    bag.setUsable(false);
    EXPECT_FALSE(bag.isBagUsable()) << "Bag should not be usable after setting to false.";
    bag.setUsable(true);
    EXPECT_TRUE(bag.isBagUsable()) << "Bag should be usable after setting to true.";
}

TEST(BagTest, StoreAndRetrievePiece) {
    Bag bag = Bag();
    Tetromino piece = Tetromino(PieceType::I);
    bag.storePiece(piece);
    EXPECT_FALSE(bag.isEmpty()) << "Bag should not be empty after storing a piece.";
    EXPECT_EQ(bag.peekPiece()->getPieceType(), piece.getPieceType()) << "Peeked piece should match stored piece.";

    Tetromino retrievedPiece = bag.retrievePiece();
    EXPECT_EQ(retrievedPiece.getPieceType(), piece.getPieceType()) << "Retrieved piece should match stored piece.";
    EXPECT_TRUE(bag.isEmpty()) << "Bag should be empty after retrieving the piece.";
}

TEST(BagTest, RetrieveFromEmptyBag) {
    Bag bag = Bag();
    EXPECT_THROW(bag.retrievePiece(), std::runtime_error) << "Retrieving from an empty bag should throw an error.";
}

TEST(BagTest, PeekFromEmptyBag) {
    Bag bag = Bag();
    EXPECT_EQ(bag.peekPiece(), nullptr) << "Peeking from an empty bag should return nullptr.";
}

TEST(BagTest, BagUnusableCannotStore) {
    Bag bag = Bag();
    bag.setUsable(false);
    Tetromino piece = Tetromino(PieceType::I);
    EXPECT_TRUE(bag.isEmpty()) << "Bag should be empty since it was unusable.";
}


