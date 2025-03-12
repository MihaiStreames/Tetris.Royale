#pragma once 

#include <iostream>

#include "loginMenu.hpp"
#include "registerMenu.hpp"
#include "settingsMenu.hpp"
#include "mainMenu.hpp"
#include "preLobbyMenu.hpp"
#include "inLobbyMenu.hpp"
#include "inGameMenu.hpp"
#include "../data/viewTestData.hpp"
enum class MenuState{
    loginMenu,
    registerMenu,
    mainMenu,
    settingsMenu,
    preLobbyMenu,
    inLobbyMenu,
    inGame,
    quitter
};

extern MenuState currMenu;