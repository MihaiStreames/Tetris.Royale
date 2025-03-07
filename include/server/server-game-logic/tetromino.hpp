
#pragma once


#include <vector>
#include <optional>
#include <stdexcept>

#include "types.hpp"



class Tetromino {


    const Position2D DEFAULT_POSITION = {0, 0};


private:

    Position2D position;
    PieceType pieceType;
    tetroShape shape;
    


public:

    Tetromino(const Position2D startPos, const PieceType type, const tetroShape& shape);
    Tetromino(const Position2D startPos, const PieceType type);
    Tetromino(const PieceType type);
    ~Tetromino() = default;

    [[nodiscard]] bool operator==(const Tetromino& other) const;
    void reset();
    
    [[nodiscard]] Position2D getPosition() const;
    [[nodiscard]] PieceType getPieceType() const;
    [[nodiscard]] const tetroShape& getShape() const;

    void setPieceType(PieceType newType);
    void setPosition(const Position2D& newPosition);
    void setShape(const tetroShape& newShape);      

    [[nodiscard]] Position2D getMovePosition(Action move) const;
    [[nodiscard]] tetroShape getRotateShape(Action rotation) const;

    // hinter here does whatever I don't really understand why this is flagged as an error but anyway it works
    [[nodiscard]] std::vector<Position2D> getAbsoluteCoordinates(std::optional<Position2D> topLeft = std::nullopt,
        const std::optional<tetroShape> &shapeOverride = std::nullopt) const;


protected:

    static tetroShape generateShapeByType(PieceType type);

};


