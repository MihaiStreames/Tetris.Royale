#ifndef WAITINGLOBBYGUI_HPP
#define WAITINGLOBBYGUI_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QtWidgets>
#include <QLineEdit>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QFontDatabase>
#include <QPixmap>
#include <QTimer>

#include <vector>

#include "MenuSwitcher.hpp" 
#include "ClientSession.hpp" 
#include "LabelWidget.hpp"
#include "ListWidget.hpp"
#include "ButtonWidget.hpp"
#include "GameScreenGUI.hpp"


class Lobby;

class WaitingLobby : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaitingLobby(ClientSession &session, QWidget *parent = nullptr);
    ~WaitingLobby();

private slots:
    void onLeaveBtnClicked();
    void changePlayerState();
    void fillPlayerListAndSpectatorList();
    void pollLobbyState();

private:
    ClientSession &session;
    MenuSwitcher buttonSwitch;
    QListWidget *playerList;
    QListWidget *spectatorList;
    LobbyState lobbyState;
    bool isReady = false;
    QTimer *lobbyPollingTimer;

    void paintEvent(QPaintEvent *event);
    void setupUi();
};

#endif // WAITINGLOBBYGUI_HPP
