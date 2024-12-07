#pragma once

#include <map>
#include <SFML/Window/Keyboard.hpp>
#include "common.hpp"

class Controller {
    std::map<sf::Keyboard::Key, Action> inputMap;
    std::map<sf::Keyboard::Key,int> invertedKeysCooldown;
    int coolDown;

public:
    Controller();
    void setupController();
    Action getInput() const;
    void invertedKeys();

    int getCoolDown() const { return coolDown; }
    void setCoolDown(const int val) { coolDown = val; }
};