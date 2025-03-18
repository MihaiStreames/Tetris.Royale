#include "MenuHandler.hpp"


// this will initialize the currentScreen variable
// at launch on the login screen

ScreenState currentScreen = ScreenState::Login;
std::atomic_bool running = true;


// this function will be called in the main loop
// and will display the current screen @ 'currentScreen'

void runTetrisClient(ClientSession &session) {


    while (currentScreen != ScreenState::Exit && running) {
        
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

    (void) session.endSession();
    // funny exit message, sticked with us during the refactoring process
    std::cout << "Thank you for playing Tetris Royale!" << std::endl;

}
