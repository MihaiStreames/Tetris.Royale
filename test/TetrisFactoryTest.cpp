#include <gtest/gtest.h>

#include "TetrisFactory.hpp"
#include "Tetromino.hpp"


TEST(TetrisFactoryTest, Constructor) {
    TetrisFactory factory = TetrisFactory();
    EXPECT_FALSE(factory.isPoolEmpty()) << "The pool should not be empty after construction.";
    EXPECT_EQ(factory.getPoolSize(), 7) << "The pool should contain 7 pieces after construction.";
}

TEST(TetrisFactoryTest, PushPiece) {
    TetrisFactory factory = TetrisFactory();
    Tetromino piece = Tetromino(PieceType::I);
    factory.pushPiece(piece);
    EXPECT_EQ(factory.getPoolSize(), 8) << "The pool should contain 8 pieces after pushing one.";
}

TEST(TetrisFactoryTest, PopPiece) {
    TetrisFactory factory = TetrisFactory();
    Tetromino piece = Tetromino(PieceType::I);
    factory.pushPiece(piece);
    EXPECT_EQ(factory.getPoolSize(), 8) << "The pool should contain 8 pieces after pushing one.";
    Tetromino poppedPiece = factory.popPiece();
    EXPECT_EQ(factory.getPoolSize(), 7) << "The pool should contain 7 pieces after popping one.";
    EXPECT_EQ(poppedPiece.getPieceType(), PieceType::I) << "The popped piece should be of type I.";
}

TEST(TetrisFactoryTest, EmptyPool) {
    TetrisFactory factory = TetrisFactory();
    for (int i = 0; i < 7; ++i) {
        (void) factory.popPiece();
    }
    EXPECT_TRUE(factory.isPoolEmpty()) << "The pool should be empty after popping all pieces.";
}

TEST(TetrisFactoryTest, Refill) {
    TetrisFactory factory = TetrisFactory();
    for (int i = 0; i < 7; ++i) {
        (void) factory.popPiece();
    }
    EXPECT_TRUE(factory.isPoolEmpty()) << "The pool should be empty after popping all pieces.";
    Tetromino piece = factory.popPiece();
    EXPECT_FALSE(factory.isPoolEmpty()) << "The pool should not be empty after refilling.";
    EXPECT_EQ(factory.getPoolSize(), 6) << "The pool should contain 6 pieces after refilling.";
    EXPECT_NE(piece.getPieceType(), PieceType::None) << "The popped piece should not be of type None.";
}

