#include "menuHandler.hpp"

// Set loginMenu comme menu initial
MenuState currMenu;

int main(){

    while(true){
        TestData data; // remplacer ici par une requete serveur
        //Selon bouton appuyé dans chaque menu -> redirection dans menu demandé
        switch (currMenu)
        {
        case MenuState::loginMenu:
            loginMenu();
            break;

        case MenuState::registerMenu:
            registerMenu();
            break;

        case MenuState::mainMenu:
            mainMenu();
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