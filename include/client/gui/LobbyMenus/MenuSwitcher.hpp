#ifndef MENUSWITCHER_HPP
#define MENUSWITCHER_HPP

#include <QPushButton>
#include <QMessageBox>
#include <QApplication>

#include "ClientSession.hpp"


// Forward declarations
class Lobby;
class WaitingLobby;


class MenuSwitcher {
public:
    template<typename MenuType, typename ArgType>
    void changeMenu(ArgType arg, ClientSession& session);
};


#endif // MENUSWITCHER_HPP
