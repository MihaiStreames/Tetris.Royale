#include <gtest/gtest.h>

#include "GameMatrix.hpp"
#include "Tetromino.hpp"


TEST(GameMatrixTest, Constructor) {
    GameMatrix matrix = GameMatrix(10, 20);
    EXPECT_EQ(matrix.getWidth(), 10) << "The width should be 10.";
    EXPECT_EQ(matrix.getHeight(), 20) << "The height should be 20.";
    EXPECT_EQ(matrix.getCurrent(), nullptr) << "The current tetromino should be null.";
    EXPECT_EQ(matrix.getBoard(), GameMatrix::generateBoardByDimension(10, 20)) << "The board should be initialized to empty.";
}

TEST(GameMatrixTest, SetCurrent) {
    GameMatrix matrix = GameMatrix(10, 20);
    Tetromino tetromino = Tetromino(PieceType::I);
    matrix.setCurrent(tetromino);
    EXPECT_EQ(matrix.getCurrent()->getPieceType(), tetromino.getPieceType()) << "The current tetromino should be set correctly.";
}

TEST(GameMatrixTest, ClearLine) {
    GameMatrix matrix = GameMatrix(10, 20);
    matrix.getBoard()[0] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // fill the first line
    EXPECT_TRUE(matrix.isLineFull(0)) << "The first line should be full before clearing.";
    matrix.clearSingleLine(0);
    EXPECT_TRUE(matrix.isLineEmpty(0)) << "The first line should be empty after clearing.";
}

TEST(GameMatrixTest, ClearFullLines) {
    GameMatrix matrix = GameMatrix(10, 20);
    matrix.getBoard()[0] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    matrix.getBoard()[4] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    EXPECT_EQ(matrix.clearFullLines(), 2) << "Two lines should be cleared.";
    EXPECT_TRUE(matrix.isLineEmpty(0)) << "The first line should be empty after clearing.";
    EXPECT_TRUE(matrix.isLineEmpty(4)) << "The second line should be empty after clearing.";
}

TEST(GameMatrixTest, AreLinesEmpty) {
    GameMatrix matrix = GameMatrix(10, 20);
    matrix.getBoard()[0] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    matrix.getBoard()[1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_FALSE(matrix.areLinesEmpty(0, 2)) << "The lines should not be empty.";
    matrix.clearSingleLine(0);
    EXPECT_TRUE(matrix.areLinesEmpty(0, 2)) << "The lines should be empty after clearing.";
}

TEST(GameMatrixTest, PushNewLinesAtBottom) {
    GameMatrix matrix = GameMatrix(10, 20);
    std::vector<std::vector<int>> newLines = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    matrix.pushNewLinesAtBottom(newLines);
    // check if the last line is the new line
    EXPECT_EQ(matrix.getBoard()[19], newLines[0]) << "The last line should be the new line.";
}

TEST(GameMatrixTest, isColliding) {
    GameMatrix matrix = GameMatrix(10, 20);
    Tetromino tetromino = Tetromino(PieceType::O);
    Position2D rpos = tetromino.getAbsoluteCoordinates()[0];
    matrix.setCurrent(tetromino);
    EXPECT_FALSE(matrix.isColliding(tetromino)) << "The tetromino should not be colliding with anything.";
    matrix.getBoard()[rpos.y][rpos.x] = 1; // simulate a block in the way
    EXPECT_TRUE(matrix.isColliding(tetromino)) << "The tetromino should be colliding with something.";
}

TEST(GameMatrixTest, move) {
    GameMatrix matrix = GameMatrix(10, 20);
    Tetromino tetromino = Tetromino(PieceType::I);
    tetromino.setPosition({0, 0});
    matrix.setCurrent(tetromino);
    EXPECT_TRUE(matrix.tryMoveRight()) << "The tetromino should be able to move right.";
    EXPECT_EQ(matrix.getCurrent()->getPosition().x, 1) << "The tetromino should have moved right.";
    EXPECT_TRUE(matrix.tryMoveDown()) << "The tetromino should be able to move down.";
    EXPECT_EQ(matrix.getCurrent()->getPosition().y, 1) << "The tetromino should have moved down.";
}

TEST(GameMatrixTest, spawn) {
    GameMatrix matrix = GameMatrix(10, 20);
    Tetromino tetromino = Tetromino(PieceType::I);
    EXPECT_TRUE(matrix.trySpawnPiece(tetromino)) << "The tetromino should be spawned successfully.";
    EXPECT_EQ(matrix.getCurrent()->getPieceType(), tetromino.getPieceType()) << "The current tetromino should be the spawned one.";
}