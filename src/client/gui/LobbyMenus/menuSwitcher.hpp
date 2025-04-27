#ifndef MENUSWITCHER_HPP
#define MENUSWITCHER_HPP

#include "ClientSession.hpp"

class Lobby;
class WaitingLobby;

class MenuSwitcher {
public:
    template<typename MenuType, typename ArgType>
    void changeMenu(ArgType arg, ClientSession& session);
};

#endif // MENUSWITCHER_H
