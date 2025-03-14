#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

#include "MenuHandler.hpp"
#include "ClientSession.hpp"
#include "Common.hpp"

// Forward declaration of the currentScreen variable which is defined in MenuHandler.cpp
extern ScreenState currentScreen;

/**
 * Shows the in-lobby screen with the player list, ready status, and lobby controls.
 * Handles transitions to the game screen when all players are ready or when the game starts.
 *
 * @param session The client session used to interact with the server
 */
void showInLobbyScreen(ClientSession &session);

/**
 * Helper function to get the game mode name from the GameMode enum
 *
 * @param mode The game mode
 * @return A string representation of the game mode
 */
std::string getGameModeName(GameMode mode);