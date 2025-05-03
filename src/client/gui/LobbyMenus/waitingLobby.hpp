#ifndef WAITINGLOBBY_HPP
#define WAITINGLOBBY_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QtWidgets>
#include <QLineEdit>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QFontDatabase>
#include <QPixmap>

#include <vector>

#include "MenuSwitcher.hpp" 
#include "ClientSession.hpp" 
#include "Label.hpp"
#include "List.hpp"
#include "Button.hpp"


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

private:
    MenuSwitcher buttonSwitch;
    QListWidget *playerList;
    QListWidget *spectatorList;
    ClientSession &session; 
    LobbyState lobbyState;
    bool isReady = false;

    void paintEvent(QPaintEvent *event);
    void setupUi();
};

#endif // WAITINGLOBBY_H
