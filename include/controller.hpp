#pragma once

#include <iostream>
#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <common.hpp>

using namespace std;

class Controller {
private:
    map<sf::Keyboard::Key,Action> inputMap;
    map<sf::Keyboard::Key,int> invertedKeysCooldown;
    int coolDown;

public:
    Controller();
    void setupController(Config config);
    Action getInput() const;
    void invertedKeys();
};