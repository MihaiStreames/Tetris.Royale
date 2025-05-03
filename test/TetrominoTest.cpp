
#include <gtest/gtest.h>

#include "Tetromino.hpp"
#include "Types.hpp"


TEST(TetrominoTest, BasicConstructor) {
    Tetromino piece = Tetromino(PieceType::I);
    EXPECT_EQ(piece.getPieceType(), PieceType::I) << "Piece type should be I";
    EXPECT_EQ(piece.getPosition().x, 0) << "Initial x position should be 0";
    EXPECT_EQ(piece.getPosition().y, 0) << "Initial y position should be 0";
    EXPECT_EQ(piece.getShape(), Tetromino(PieceType::I).getShape()) << "Initial shape should be I shape";
}

TEST(TetrominoTest, ParameterizedConstructor) {
    Position2D startPos = {4, 5};
    Tetromino piece = Tetromino(startPos, PieceType::T);
    EXPECT_EQ(piece.getPieceType(), PieceType::T) << "Piece type should be T";
    EXPECT_EQ(piece.getPosition().x, startPos.x) << "Initial x position should be 4";
    EXPECT_EQ(piece.getPosition().y, startPos.y) << "Initial y position should be 5";
    EXPECT_EQ(piece.getShape(), Tetromino(PieceType::T).getShape()) << "Initial shape should be T shape";
}

TEST(TetrominoTest, MovePosition) {
    Tetromino piece = Tetromino(PieceType::L);
    Position2D newPos = piece.getMovePosition(Action::MoveRight);
    EXPECT_EQ(newPos.x, 1) << "New x position should be 1 after moving right";
    EXPECT_EQ(newPos.y, 0) << "New y position should be 0 after moving right";

    newPos = piece.getMovePosition(Action::MoveDown);
    EXPECT_EQ(newPos.x, 0) << "New x position should be 0 after moving down";
    EXPECT_EQ(newPos.y, 1) << "New y position should be 1 after moving down";
}

TEST(TetrominoTest, RotateShape) {
    Tetromino piece = Tetromino(PieceType::S);
    tetroShape rotatedShape = piece.getRotateShape(Action::RotateRight);
    tetroShape expectedShape = {
        {false, true, false},
        {false, true, true},
        {false, false, true},
    };
    EXPECT_EQ(rotatedShape, expectedShape) << "Rotated shape should match expected S shape after rotation";
}

TEST(TetrominoTest, GetAbsoluteCoordinates) {
    Tetromino piece = Tetromino(PieceType::O);
    std::vector<Position2D> coords = piece.getAbsoluteCoordinates();
    EXPECT_EQ(coords.size(), 4) << "Should have 4 coordinates for O piece";
    EXPECT_EQ(coords[0].x, 0) << "First coordinate x should be 0";
    EXPECT_EQ(coords[0].y, 0) << "First coordinate y should be 0";
    EXPECT_EQ(coords[1].x, 1) << "Second coordinate x should be 1";
    EXPECT_EQ(coords[1].y, 0) << "Second coordinate y should be 0";
    EXPECT_EQ(coords[2].x, 0) << "Third coordinate x should be 0";
    EXPECT_EQ(coords[2].y, 1) << "Third coordinate y should be 1";
    EXPECT_EQ(coords[3].x, 1) << "Fourth coordinate x should be 1";
    EXPECT_EQ(coords[3].y, 1) << "Fourth coordinate y should be 1";
}

TEST(TetrominoTest, Setters) {
    Tetromino piece = Tetromino(PieceType::I);
    Position2D newPos = {3, 4};
    piece.setPosition(newPos);
    EXPECT_EQ(piece.getPosition().x, newPos.x) << "X position should be updated to 3";
    EXPECT_EQ(piece.getPosition().y, newPos.y) << "Y position should be updated to 4";

    tetroShape newShape = {
        {true, true},
        {true, true},
    };
    piece.setShape(newShape);
    EXPECT_EQ(piece.getShape(), newShape) << "Shape should be updated to new shape";
}

TEST(TetrominoTest, EqualityOperator) {
    Tetromino piece1 = Tetromino(PieceType::J);
    Tetromino piece2 = Tetromino(PieceType::J);
    EXPECT_TRUE(piece1 == piece2) << "Two identical pieces should be equal";

    piece2.setPosition({1, 1});
    EXPECT_FALSE(piece1 == piece2) << "Pieces with different positions should not be equal";
}

TEST(TetrominoTest, Reset) {
    Tetromino piece = Tetromino(PieceType::Z);
    piece.setPosition({5, 5});
    piece.reset();
    EXPECT_EQ(piece.getPosition().x, 0) << "Position should reset to 0";
    EXPECT_EQ(piece.getPosition().y, 0) << "Position should reset to 0";
    EXPECT_EQ(piece.getPieceType(), PieceType::Z) << "Piece type should remain Z after reset";
}

TEST(TetrominoTest, InvalidPieceType) {
    EXPECT_THROW(Tetromino piece = Tetromino(PieceType::None), std::invalid_argument) << "Creating a Tetromino with None type should throw an exception";
}

TEST(TetrominoTest, RotationFullCircle) {
    Tetromino piece(PieceType::T);
    tetroShape originalShape = piece.getShape();

    // 4 rotations should return to the original shape
    for (int i = 0; i < 4; ++i) {
        tetroShape rotatedShape = piece.getRotateShape(Action::RotateRight);
        piece.setShape(rotatedShape);
    }

    EXPECT_EQ(piece.getShape(), originalShape) << "Rotating a piece 4 times should return to the original shape";
}



