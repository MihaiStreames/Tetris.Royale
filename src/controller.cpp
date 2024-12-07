#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include "controller.hpp"

using namespace std;

void Controller::setupController(Config config) {
    // flèches pour les mouvements Left,Right,Down du tetromino
    inputMap[sf::Keyboard::Left] = Action::Move
    inputMap[sf::Keyboard::Right] = Action::Move
    inputMap[sf::Keyboard::Down] = Action::Move

    // R et L pour rotation Right et Left
    inputMap[sf::Keyboard::R] = Action::Rotation;
    inputMap[sf::Keyboard::L] = Action::Rotation;

    // B pour l'utilisation du Bag
    inputMap[sf::Keyboard::B] = Action::UseBag;
}

// Pas 100% sur du getInput et invertedKeys
Action getInput() const {
    for (const auto& pair: inputMap) {
        if (sf::Keyboard::isKeyPressed(pair.first)) {
        // Mouvement classique
            if (pair.second == Action::Move) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    return Move::Left;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    return Move::Right;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    return Move::Down;
                }
            }
            // Rotation
            else if (pair.second == Action::Rotation) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
                    return Rotation::Left;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                    return Rotation::Right;
                }
            }
            // Bag
            else if(pair.second == Action::UseBag) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
                    return Action::UseBag;
                }
            }
        }
    }
    //Aucune touche valide entrée par l'utilisateur
    return Action::NULL;
}

void Controller::invertedKeys() {
    // inversion des touches droite et gauche
    inputMap[sf::Keyboard::Left] = Action::Move;
    inputMap[sf::Keyboard::Right] = Action::Move;
}