
#include <iostream>
#include <string>
#include "ClientSession.hpp"


void printMenu() {

    std::cout << "Menu:" << std::endl;
    std::cout << "1. Login Player" << std::endl;
    std::cout << "2. Register Player" << std::endl;
    std::cout << "3. Fetch Player Data" << std::endl;
    std::cout << "4. Update Player" << std::endl;
    std::cout << "5. Post Score" << std::endl;
    std::cout << "6. Send Friend Request" << std::endl;
    std::cout << "7. Accept Friend Request" << std::endl;
    std::cout << "8. Decline Friend Request" << std::endl;
    std::cout << "9. Remove Friend" << std::endl;
    std::cout << "10. Start Session" << std::endl;
    std::cout << "11. End Session" << std::endl;
    std::cout << "12. Create and Join Lobby" << std::endl;
    std::cout << "13. Join Lobby" << std::endl;
    std::cout << "14. Spectate Lobby" << std::endl;
    std::cout << "15. Leave Lobby" << std::endl;
    std::cout << "16. Ready Up" << std::endl;
    std::cout << "17. Unready Up" << std::endl;
    std::cout << "18. Send Key Stroke" << std::endl;
    std::cout << "19. Get GameState as a Player" << std::endl;
    std::cout << "20. Get Player Status" << std::endl;
    std::cout << "21. Exit" << std::endl;

}


int main() {

    ClientSession clientSession(true);

    int choice;
    std::string username, password, friendID, lobbyID, newName, newPassword;
    int score, maxPlayers;
    bool isPublic;
    bool running = true;

    while (running) {

        printMenu();

        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {

            // logic
            case 1:
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter password: ";
                std::cin >> password;
                clientSession.loginPlayer(username, password);
                break;

            // register
            case 2:
                std::cout << "Enter username: ";
                std::cin >> username;
                std::cout << "Enter password: ";
                std::cin >> password;
                clientSession.registerPlayer(username, password);
                break;

            // fetch data
            case 3:
                // TODO fetch data
                clientSession.fetchPlayerData();
                break;

            // update acc
            case 4:
                std::cout << "Enter new username: ";
                std::cin >> newName;
                std::cout << "Enter new password: ";
                std::cin >> newPassword;
                clientSession.updatePlayer(newName, newPassword);
                break;
            
            // post score
            case 5:
                std::cout << "Enter score: ";
                std::cin >> score;
                clientSession.postScore(score);
                break;

            // send friend request
            case 6:
                std::cout << "Enter friend ID: ";
                std::cin >> friendID;
                clientSession.sendFriendRequest(friendID);
                break;

            // accept friend request
            case 7:
                std::cout << "Enter friend ID: ";
                std::cin >> friendID;
                clientSession.acceptFriendRequest(friendID);
                break;

            // decline friend request
            case 8:
                std::cout << "Enter friend ID: ";
                std::cin >> friendID;
                clientSession.declineFriendRequest(friendID);
                break;

            // remove friend
            case 9:
                std::cout << "Enter friend ID: ";
                std::cin >> friendID;
                clientSession.removeFriend(friendID);
                break;

            // start session
            case 10:
                clientSession.startSession();
                break;

            // end session
            case 11:
                clientSession.endSession();
                break;

            // create and join lobby
            case 12:
                std::cout << "Enter game mode (0 for CLASSIC, 1 for ROYALE, 2 for DUEL): ";
                std::cin >> choice;
                std::cout << "Enter max players: ";
                std::cin >> maxPlayers;
                std::cout << "Is public (1 for yes, 0 for no): ";
                std::cin >> isPublic;
                clientSession.createAndJoinLobby(static_cast<GameMode>(choice), maxPlayers, isPublic);
                break;

            // join lobby
            case 13:
                std::cout << "Enter lobby ID: ";
                std::cin >> lobbyID;
                clientSession.joinLobby(lobbyID);
                break;

            // spectate lobby
            case 14:
                std::cout << "Enter lobby ID: ";
                std::cin >> lobbyID;
                clientSession.spectateLobby(lobbyID);
                break;

            // leave lobby
            case 15:
                clientSession.leaveLobby();
                break;

            // ready up
            case 16:
                clientSession.readyUp();
                break;

            // unready up
            case 17:
                clientSession.unreadyUp();
                break;

            // send key stroke
            case 18: {
                
                // create a map of all the actions linked with numbers
                std::map<int, Action> actionMap = {
                    {0, Action::MoveLeft},
                    {1, Action::MoveRight},
                    {2, Action::MoveDown},
                    {3, Action::RotateLeft},
                    {4, Action::RotateRight},
                    {5, Action::UseBag},
                    {6, Action::UseBonus},
                    {7, Action::UseMalus},
                    {8, Action::InstantFall},
                    {9, Action::SeePreviousOpponent},
                    {10, Action::SeeNextOpponent},
                };

                std::cout << "Enter action (0 for MoveLeft, 1 for MoveRight, 2 for MoveDown, 3 for RotateLeft, 4 for RotateRight, 5 for UseBag, 6 for UseBonus, 7 for UseMalus, 8 for InstantFall), 9 for SeePreviousOpponent, 10 for SeeNextOpponent: ";
                std::cin >> choice;

                // send the key stroke
                clientSession.sendKeyStroke(actionMap[choice]);
                break;

            }

            // get gamestate player view
            case 19: {

                PlayerState state = clientSession.getPlayerState();
                
                // print the board
                for (int i = 0; i < state.playerGrid.size(); i++) {
                    std::cout << "|";
                    for (int j = 0; j < state.playerGrid[i].size(); j++) {
                        std::cout << (state.playerGrid[i][j] ? "█" : ".");
                    }
                    std::cout << "|" << "             " << "|";
                    for (int j = 0; j < state.targetGrid[i].size(); j++) {
                        std::cout << (state.targetGrid[i][j] ? "x" : ".");
                    }
                    std::cout << "|" << std::endl;
                }

                break;
            }
            
            // get player status
            case 20: {
                std::cout << "Enter username: ";
                std::cin >> username;
                ClientStatus status = clientSession.getPlayerStatus(username);
                std::cout << "Status: " << static_cast<int>(status) << std::endl;
                break;
            }

            // exit
            case 21:
                running = false;
                break;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
            
        }
    }

    return 0;

}

