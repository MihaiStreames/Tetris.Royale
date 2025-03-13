#include "MainMenu.hpp"
#include "Common.hpp"

using namespace ftxui;

void showMainMenu(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Fetch player data
    session.fetchPlayerData();

    // Tab selection
    int activeTab = 0;
    std::vector<std::string> tabs = {"Home", "Friends", "Messages", "Leaderboard"};
    auto tabToggle = Toggle(&tabs, &activeTab);

    // Friends tab
    std::vector<std::string> &friendList = session.getFriendList();
    std::vector<std::string> &pendingRequests = session.getPendingFriendRequests();

    // Friend request management
    std::string friendToAdd;
    auto friendInput = Input(&friendToAdd, "Friend's username");

    auto addFriendButton = Button("Add Friend", [&] {
        if (!friendToAdd.empty()) {
            session.sendFriendRequest(friendToAdd);
            friendToAdd.clear();
            // Refresh data
            session.fetchPlayerData();
        }
    });

    // Messaging
    int selectedFriendIndex = 0;
    auto friendSelector = Menu(&friendList, &selectedFriendIndex);

    std::string messageInput;
    auto messageTextInput = Input(&messageInput, "Type message");

    auto sendMessageButton = Button("Send", [&] {
        if (!messageInput.empty() && !friendList.empty()) {
            std::string recipient = friendList[selectedFriendIndex];
            session.sendMessage(recipient, messageInput);
            messageInput.clear();
        }
    });

    // Navigation buttons
    auto playButton = Button("Play", [&] {
        currentScreen = ScreenState::LobbyBrowser;
        screen.Exit();
    });

    auto logoutButton = Button("Logout", [&] {
        session.endSession();
        currentScreen = ScreenState::Login;
        screen.Exit();
    });

    // Notifications for friend requests
    std::vector<Component> requestComponents;
    for (size_t i = 0; i < pendingRequests.size(); i++) {
        // Using a copy of i to avoid capture issues
        size_t index = i;

        auto acceptButton = Button("Accept", [&, index] {
            if (index < pendingRequests.size()) {
                session.acceptFriendRequest(pendingRequests[index]);
                session.fetchPlayerData();
            }
        });

        auto declineButton = Button("Decline", [&, index] {
            if (index < pendingRequests.size()) {
                session.declineFriendRequest(pendingRequests[index]);
                session.fetchPlayerData();
            }
        });

        auto container = Container::Horizontal({acceptButton, declineButton});
        requestComponents.push_back(container);
    }

    // Main container for interactive components
    auto container = Container::Vertical({
        tabToggle,
        playButton,
        logoutButton,
        friendSelector,
        friendInput,
        addFriendButton,
        messageTextInput,
        sendMessageButton
    });

    // Add friend request components
    for (auto &comp: requestComponents) {
        container->Add(comp);
    }

    // Main renderer
    auto renderer = Renderer(container, [&] {
        // Get fresh data for leaderboard
        std::vector<PlayerScore> leaderboard = session.getLeaderboard(10);

        // Content based on active tab
        Element content;

        switch (activeTab) {
            case 0: // Home
                content = vbox({
                    text("Welcome, " + session.getUsername()) | bold,
                    text("Best Score: " + std::to_string(session.getBestScore())),
                    separator(),
                    playButton->Render() | center
                });
                break;

            case 1: // Friends
            {
                // Friend list
                Elements friendElements;
                if (friendList.empty()) {
                    friendElements.push_back(text("You have no friends yet"));
                } else {
                    for (const auto &friendName: friendList) {
                        friendElements.push_back(text(friendName));
                    }
                }

                // Friend requests
                Elements requestElements;
                for (size_t i = 0; i < pendingRequests.size(); i++) {
                    requestElements.push_back(
                        hbox({
                            text("Request from: " + pendingRequests[i]),
                            requestComponents[i]->Render()
                        })
                    );
                }

                content = vbox({
                    text("Friends") | bold,
                    vbox(friendElements) | border,
                    separator(),
                    text("Friend Requests") | bold,
                    vbox(requestElements) | border,
                    separator(),
                    hbox({
                        friendInput->Render(),
                        addFriendButton->Render()
                    })
                });
            }
            break;

            case 2: // Messages
            {
                if (friendList.empty()) {
                    content = text("Add friends to start messaging");
                } else {
                    std::string selectedFriend = friendList[selectedFriendIndex];
                    std::vector<ChatMessage> messages = session.getPlayerMessages(selectedFriend);

                    Elements messageElements;
                    if (messages.empty()) {
                        messageElements.push_back(text("No messages yet"));
                    } else {
                        for (const auto &msg: messages) {
                            messageElements.push_back(text(msg.from + ": " + msg.text));
                        }
                    }

                    content = vbox({
                        text("Chat with: " + selectedFriend) | bold,
                        friendSelector->Render(),
                        separator(),
                        vbox(messageElements) | border | yframe,
                        hbox({
                            messageTextInput->Render(),
                            sendMessageButton->Render()
                        })
                    });
                }
            }
            break;

            case 3: // Leaderboard
            {
                Elements leaderboardElements;
                leaderboardElements.push_back(
                    hbox({
                        text("Rank") | size(WIDTH, EQUAL, 8),
                        text("Player") | size(WIDTH, EQUAL, 15),
                        text("Score") | size(WIDTH, EQUAL, 10)
                    }) | bold
                );

                for (const auto &entry: leaderboard) {
                    leaderboardElements.push_back(
                        hbox({
                            text(std::to_string(entry.rank)) | size(WIDTH, EQUAL, 8),
                            text(entry.name) | size(WIDTH, EQUAL, 15),
                            text(std::to_string(entry.score)) | size(WIDTH, EQUAL, 10)
                        })
                    );
                }

                content = vbox({
                    text("Leaderboard") | bold,
                    vbox(leaderboardElements) | border
                });
            }
            break;
        }

        return vbox({
                   text("TETRIS ROYALE") | bold | center,
                   separator(),
                   tabToggle->Render(),
                   content,
                   separator(),
                   hbox({
                       filler(),
                       logoutButton->Render(),
                       filler()
                   })
               }) | border | color(Color::Green);
    });

    // Main loop
    screen.Loop(renderer);
}
