#include "MenuHandler.hpp"

// Set loginMenu comme menu initial
MenuState currMenu;

int
main()
{

    // instantiate the clientSession here
    ClientSession clientSession = ClientSession(MASTER_SERVER_IP, LOBBY_SERVER_PORT, DB_SERVER_PORT);

    while (true)
    {
        TestData data; // remplacer ici par une requete serveur
        // Selon bouton appuyé dans chaque menu -> redirection dans menu demandé
        switch (currMenu)
        {
        case MenuState::loginMenu:
            loginMenu(clientSession);
            break;

        case MenuState::registerMenu:
            registerMenu(clientSession);
            break;

        case MenuState::mainMenu:
            mainMenu(clientSession);
            break;

        case MenuState::settingsMenu:
            settingsMenu();
            break;
        case MenuState::inLobbyMenu:
            inLobbyMenu();
            break;

        case MenuState::preLobbyMenu:
            preLobbyMenu();
            break;

        case MenuState::inGame:
            inGameMenu(data);
            break;
        case MenuState::quitter:
            return 0;
        }
    }
    return 0;
}