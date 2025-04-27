#include "mainWindow.hpp"

ScreenState currentScreen = ScreenState::Login;
std::atomic_bool running = true;

MainWindow::MainWindow(ClientSession &session, QWidget *parent) : session(session), QMainWindow(parent) {
    
    stackedWidget = new QStackedWidget(this);
    
    loginWidget = new LoginScreen(session);
    registerWidget = new RegisterScreen(session);
    /* mainMenuWidget = new mainMenu(session);
    settingsWidget = new settingsMenu(session);
    lobbyWidget = new Lobby(session);
    waitingLobbyWidget = new WaitingLobby(session);
    inGameWidget = new inGameMenu(session); */

    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);
    /*stackedWidget->addWidget(mainMenuWidget);
    stackedWidget->addWidget(settingsWidget);
    stackedWidget->addWidget(lobbyWidget);
    stackedWidget->addWidget(waitingLobbyWidget);
    stackedWidget->addWidget(inGameWidget);*/

    setCentralWidget(stackedWidget);

    while (currentScreen != ScreenState::Exit && running) {
        
        switch (currentScreen) {

            case ScreenState::Login:
                stackedWidget->setCurrentWidget(loginWidget); // Affiche le loginMenu
                break;

            case ScreenState::Register:
                stackedWidget->setCurrentWidget(registerWidget); // Affiche le registerMenu
                break;

            /*case ScreenState::MainMenu:
                stackedWidget->setCurrentWidget(mainMenuWidget); // Affiche le mainMenu
                break;

            case ScreenState::SettingsMenu:
                stackedWidget->setCurrentWidget(settingsWidget); // Affiche les settings
                break;

            case ScreenState::Lobby:
                stackedWidget->setCurrentWidget(lobbyWidget); // Affiche le lobby
                break;

            case ScreenState::WaitingLobby:
                stackedWidget->setCurrentWidget(waitingLobbyWidget); // Affiche le waitingLobby
                break;

            case ScreenState::InGame:
                stackedWidget->setCurrentWidget(inGameWidget); // Affiche le inGameMenu
                break;

            case ScreenState::Exit:
                running = false; // Arrête la boucle
                break; */

            default:
                currentScreen = ScreenState::Exit;
                break;
        };
        
    }
    (void) session.endSession();
    // funny exit message, sticked with us during the refactoring process
    std::cout << "Thank you for playing Tetris Royale!" << std::endl;
}
