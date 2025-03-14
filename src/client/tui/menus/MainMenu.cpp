#include "MainMenu.hpp"
#include "Common.hpp"

using namespace ftxui;

void showMainMenu(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Fetch player data initially
    (void) session.fetchPlayerData();

    // Tab selection
    int activeTab = 0;
    int previousTab = 0; // Track previous tab to detect changes
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
            (void) session.sendFriendRequest(friendToAdd);
            friendToAdd.clear();
            // Refresh data
            (void) session.fetchPlayerData();
        }
    });

    // Messaging
    int selectedFriendIndex = 0;
    auto friendSelector = Menu(&friendList, &selectedFriendIndex);

    // Track friend selection changes to refresh messages
    int previousFriendIndex = -1; // Force initial refresh

    std::string messageInput;
    InputOption messageInputOption;
    messageInputOption.on_change = [&] {
        if (!friendList.empty() && activeTab == 2) {
            std::string selectedFriend = friendList[selectedFriendIndex];
        }
    };
    auto messageTextInput = Input(&messageInput, "Type message", messageInputOption);

    auto sendMessageButton = Button("Send", [&] {
        if (!messageInput.empty() && !friendList.empty()) {
            std::string recipient = friendList[selectedFriendIndex];
            (void) session.sendMessage(recipient, messageInput);
            messageInput.clear();

            // Poll multiple times after sending to ensure message appears (magic numbers, sorta)
            for (int i = 0; i < 3; i++) {
                (void) session.getPlayerMessages(recipient);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });

    // Navigation buttons
    auto playButton = Button("Play", [&] {
        currentScreen = ScreenState::LobbyBrowser;
        screen.Exit();
    });

    auto logoutButton = Button("Logout", [&] {
        if (session.endSession() == StatusCode::SUCCESS) {
            currentScreen = ScreenState::Login;
            screen.Exit();
        }
    });

    // Notifications for friend requests
    std::vector<Component> requestComponents;
    for (size_t i = 0; i < pendingRequests.size(); i++) {
        // Using a copy of i to avoid capture issues
        size_t index = i;

        auto acceptButton = Button("Accept", [&, index] {
            if (index < pendingRequests.size()) {
                (void) session.acceptFriendRequest(pendingRequests[index]);
                (void) session.fetchPlayerData();
            }
        });

        auto declineButton = Button("Decline", [&, index] {
            if (index < pendingRequests.size()) {
                (void) session.declineFriendRequest(pendingRequests[index]);
                (void) session.fetchPlayerData();
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

    // Variables for periodic refresh
    int refreshCounter = 0;
    int messageRefreshCounter = 0;
    constexpr int REFRESH_INTERVAL = 100; // Refresh every 100 render cycles
    constexpr int MESSAGE_REFRESH_INTERVAL = 5; // Poll for messages very frequently (almost constant polling)

    // Main renderer
    auto renderer = Renderer(container, [&] {
        // Check for tab change and refresh data if needed
        if (activeTab != previousTab) {
            (void) session.fetchPlayerData();

            // If switching to Messages tab, refresh messages for the selected friend
            if (activeTab == 2 && !friendList.empty()) {
                std::string selectedFriend = friendList[selectedFriendIndex];
                (void) session.getPlayerMessages(selectedFriend); // This updates the internal message cache
            }

            previousTab = activeTab;
        }

        // Periodic refresh for player data
        refreshCounter++;
        if (refreshCounter >= REFRESH_INTERVAL) {
            (void) session.fetchPlayerData();
            refreshCounter = 0;
        }

        // Almost constant polling for messages when in Messages tab
        if (activeTab == 2 && !friendList.empty()) {
            messageRefreshCounter++;
            if (messageRefreshCounter >= MESSAGE_REFRESH_INTERVAL) {
                std::string selectedFriend = friendList[selectedFriendIndex];
                (void) session.getPlayerMessages(selectedFriend); // This updates the internal message cache
                messageRefreshCounter = 0;
            }

            // Even poll on every frame when user is interacting with message input
            if (messageTextInput->Focused()) {
                std::string selectedFriend = friendList[selectedFriendIndex];
                (void) session.getPlayerMessages(selectedFriend);
            }

            if (activeTab == 2 && selectedFriendIndex != previousFriendIndex && !friendList.empty()) {
                std::string selectedFriend = friendList[selectedFriendIndex];
                // Poll immediately when changing conversation
                (void) session.getPlayerMessages(selectedFriend);
                previousFriendIndex = selectedFriendIndex;
            }
        }

        // Rebuild friend request components if needed
        if (requestComponents.size() != pendingRequests.size()) {
            requestComponents.clear();
            for (size_t i = 0; i < pendingRequests.size(); i++) {
                size_t index = i;
                auto acceptButton = Button("Accept", [&, index] {
                    if (index < pendingRequests.size()) {
                        (void) session.acceptFriendRequest(pendingRequests[index]);
                        (void) session.fetchPlayerData();
                    }
                });

                auto declineButton = Button("Decline", [&, index] {
                    if (index < pendingRequests.size()) {
                        (void) session.declineFriendRequest(pendingRequests[index]);
                        (void) session.fetchPlayerData();
                    }
                });

                auto container = Container::Horizontal({acceptButton, declineButton});
                requestComponents.push_back(container);
            }
        }

        // Get fresh data for leaderboard if on that tab
        std::vector<PlayerScore> leaderboard;
        if (activeTab == 3) {
            leaderboard = session.getLeaderboard(10);
        }

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
                    for (const auto &friendID: friendList) {
                        std::string friendUsername = session.getFriendUsername(friendID);
                        friendElements.push_back(text(friendUsername));
                    }
                }

                // Friend requests
                Elements requestElements;
                for (size_t i = 0; i < pendingRequests.size(); i++) {
                    std::string requesterUsername = session.getRequestUsername(pendingRequests[i]);
                    requestElements.push_back(
                        hbox({
                            text("Request from: " + requesterUsername),
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
                    std::string friendUsername = session.getFriendUsername(selectedFriend);
                    // Force message refresh on every frame when in message tab
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
                        text("Chat with: " + friendUsername) | bold,
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
