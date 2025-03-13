#include "MenuHandler.hpp"
#include "Common.hpp"
#include <iostream>

// Initialize the current screen to Login
ScreenState currentScreen = ScreenState::Login;

void runTetrisClient() {
    // Create the client session
    ClientSession session(MASTER_SERVER_IP, LOBBY_SERVER_PORT, DB_SERVER_PORT);

    // Main application loop
    while (currentScreen != ScreenState::Exit) {
        switch (currentScreen) {
            case ScreenState::Login:
                showLoginScreen(session);
            break;

            case ScreenState::Register:
                showRegisterScreen(session);
            break;

            case ScreenState::MainMenu:
                showMainMenu(session);
            break;

            case ScreenState::LobbyBrowser:
                showLobbyBrowser(session);
            break;

            case ScreenState::InLobby:
                showInLobbyScreen(session);
            break;

            case ScreenState::InGame:
                showGameScreen(session);
            break;

            default:
                currentScreen = ScreenState::Exit;
            break;
        }
    }

    std::cout << "Thank you for playing Tetris Royale!" << std::endl;
}

// The main entry point
int main() {
    runTetrisClient();
    return 0;
}