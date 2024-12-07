#include <SFML/Window/Keyboard.hpp>
#include "controller.hpp"

Controller::Controller() : coolDown(0) {}

void Controller::setupController() {
    inputMap[sf::Keyboard::Left] = MoveLeft;
    inputMap[sf::Keyboard::Right] = MoveRight;
    inputMap[sf::Keyboard::Down] = MoveDown;
    inputMap[sf::Keyboard::R] = RotateRight;
    inputMap[sf::Keyboard::L] = RotateLeft;
    inputMap[sf::Keyboard::B] = UseBag;
}

Action Controller::getInput() const {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) return MoveLeft;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) return MoveRight;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) return MoveDown;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) return RotateLeft;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) return RotateRight;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) return UseBag;
    return None;
}

void Controller::invertedKeys() {
    Action leftAction = inputMap[sf::Keyboard::Left];
    inputMap[sf::Keyboard::Left] = inputMap[sf::Keyboard::Right];
    inputMap[sf::Keyboard::Right] = leftAction;
}