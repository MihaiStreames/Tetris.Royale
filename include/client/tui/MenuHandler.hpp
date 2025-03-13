#pragma once

#include <iostream>

#include "InGameMenu.hpp"
#include "InLobbyMenu.hpp"
#include "LoginMenu.hpp"
#include "MainMenu.hpp"
#include "PreLobbyMenu.hpp"
#include "RegisterMenu.hpp"
#include "SettingsMenu.hpp"
#include "ViewTestData.hpp"

#include "ClientSession.hpp"

enum class MenuState
{
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