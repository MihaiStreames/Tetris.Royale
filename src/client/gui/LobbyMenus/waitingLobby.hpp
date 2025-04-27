#ifndef WAITINGLOBBY_HPP
#define WAITINGLOBBY_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QFontDatabase>
#include <QPixmap>

#include "menuSwitcher.hpp" 
#include "ClientSession.hpp" 
#include "label.hpp"
#include "list.hpp"
#include "button.hpp"


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
