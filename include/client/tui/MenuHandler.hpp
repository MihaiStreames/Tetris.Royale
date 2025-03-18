#pragma once


#include <iostream>
#include <string>

#include "ClientSession.hpp"
#include "Common.hpp"


// Enum for the different screens in the application
enum class ScreenState {
    Login,
    Register,
    MainMenu,
    LobbyBrowser,
    InLobby,
    InGame,
    Exit
};

// Forward declarations of screen functions
void showLoginScreen(ClientSession& session);
void showRegisterScreen(ClientSession& session);
void showMainMenu(ClientSession& session);
void showLobbyBrowser(ClientSession& session);
void showInLobbyScreen(ClientSession& session);
void showGameScreen(ClientSession& session, Config &config);

// Global current screen state
extern ScreenState currentScreen;


// Main application loop
void runTetrisClient(ClientSession &session, Config &config);
