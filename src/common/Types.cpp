#include "Types.hpp"

std::vector<TypePowerUps> malusVector = {
    TypePowerUps::blockControls, TypePowerUps::invertedControls,
    TypePowerUps::thunderStrike, TypePowerUps::darkMode,
    TypePowerUps::fastPieces,
};

std::vector<TypePowerUps> bonusVector = {
    TypePowerUps::slowPieces,
    TypePowerUps::singleBlocks,
};

std::string actionToString(Action action) {
    switch (action) {
        case Action::MoveLeft:
            return "MoveLeft";
        case Action::MoveRight:
            return "MoveRight";
        case Action::MoveDown:
            return "MoveDown";
        case Action::RotateLeft:
            return "RotateLeft";
        case Action::RotateRight:
            return "RotateRight";
        case Action::InstantFall:
            return "InstantFall";
        case Action::UseBag:
            return "UseBag";
        case Action::UseMalus:
            return "UseMalus";
        case Action::UseBonus:
            return "UseBonus";
        case Action::SeePreviousOpponent:
            return "SeePreviousOpponent";
        case Action::SeeNextOpponent:
            return "SeeNextOpponent";
        default:
            return "None";
    }
}
