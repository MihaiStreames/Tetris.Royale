#pragma once

#include <QMainWindow>
#include <QStackedWidget>

#include <iostream>
#include <string>
#include "LoginScreen.hpp"
#include "RegisterScreen.hpp"
#include "ClientSession.hpp"

//include all menus 

enum class ScreenState {
    Login,
    Register,
    MainMenu,
    SettingsMenu,
    Lobby,
    WaitingLobby,
    InGame,
    Exit
};

extern ScreenState currentScreen;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ClientSession &session, QWidget *parent = nullptr);

private:
    ClientSession &session;

    QStackedWidget *stackedWidget;

    QWidget *loginWidget;
    QWidget *registerWidget;
    // mainMenu *mainMenuWidget;
    // settingsMenu *settingsWidget;
    // Lobby *lobbyWidget;
    // WaitingLobby *waitingLobbyWidget;
    // inGameMenu *inGameWidget;

};
