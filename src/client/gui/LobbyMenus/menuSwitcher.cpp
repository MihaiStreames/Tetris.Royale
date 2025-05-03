
#include "MenuSwitcher.hpp"
#include "Lobby.hpp" 
#include "WaitingLobby.hpp" 


template<typename MenuType, typename ArgType>
void MenuSwitcher::changeMenu(ArgType arg, ClientSession& session) {
    MenuType* nextMenu = new MenuType(session);
    arg->close();
    nextMenu->show();
}

// Explicit template instantiations for known types
template void MenuSwitcher::changeMenu<Lobby, QWidget*>(QWidget* arg, ClientSession& session);
template void MenuSwitcher::changeMenu<WaitingLobby, QWidget*>(QWidget* arg, ClientSession& session);

// Add missing explicit template instantiations
template void MenuSwitcher::changeMenu<WaitingLobby, Lobby*>(Lobby* arg, ClientSession& session);
template void MenuSwitcher::changeMenu<Lobby, WaitingLobby*>(WaitingLobby* arg, ClientSession& session);
