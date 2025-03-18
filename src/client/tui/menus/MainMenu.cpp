#include "MainMenu.hpp"


using namespace ftxui;

void showMainMenu(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();
    currentScreen = ScreenState::Exit;

    // Fetch player data initially
    (void) session.fetchPlayerData();

    // Tab selection
    int activeTab = 0;
    const std::vector<std::string> tabs = {"Home", "Friends", "Messages", "Leaderboard"};
    auto tabToggle = Toggle(&tabs, &activeTab);

    // Friends tab
    std::vector<std::string> &friendList = session.getFriendList();
    const std::vector<std::string> &pendingRequests = session.getPendingFriendRequests();

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
    std::vector<std::string> friendNames;
    for (const auto &id : friendList) {
        friendNames.push_back(session.getFriendUsername(id));
    }
    int selectedFriendIndex = 0;
    auto friendSelector = Menu(&friendNames, &selectedFriendIndex);

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
            const std::string recipient = friendList[selectedFriendIndex];
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
    const auto container = Container::Vertical({
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
    for (const auto &comp: requestComponents) {
        container->Add(comp);
    }

    // Main renderer
    const auto renderer = Renderer(container, [&] {
        // Rebuild friend request components if pendingRequests have changed
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

                auto reqContainer = Container::Horizontal({acceptButton, declineButton});
                requestComponents.push_back(reqContainer);
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
                        // we get the username from the friendID and it's status
                        std::string friendUsername = session.getFriendUsername(friendID);
                        std::string status = getClientStatusString(session.getClientStatus(friendUsername));
                        friendElements.push_back(
                            hbox({
                                text(friendUsername),
                                text(" (" + status + ")")
                            })
                        );
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
                    const std::string selectedFriendID = friendList[selectedFriendIndex];
                    const std::string friendUsername   = friendNames[selectedFriendIndex];
                    std::vector<ChatMessage> messages = session.getPlayerMessages(selectedFriendID);

                    Elements messageElements;
                    if (messages.empty()) {
                        messageElements.push_back(text("No messages yet"));
                    } else {
                        for (const auto &it: messages) {
                            messageElements.push_back(text(it.from + ": " + it.text));
                        }
                    }

                    auto messagesContainer = vbox({
                                                vbox(messageElements)
                                            }) | border | yframe | size(HEIGHT, EQUAL, 10);

                    content = vbox({
                        text("Chat with: " + friendUsername) | bold,
                        friendSelector->Render(),
                        separator(),
                        messagesContainer,
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
            default:
                break;
        }

        return vbox({
                   text("TETRIS ROYALE") | bold | center,
                   separator(),
                   tabToggle->Render() | border,
                   content,
                   separator(),
                   hbox({
                       filler(),
                       logoutButton->Render(),
                       filler()
                   })
               }) | border | color(Color::Green);
    });

    std::atomic_bool running{true};
    std::mutex mtx;
    std::condition_variable cv;

    // Counters for polling intervals
    int fetchDataCounter = 0;
    int fetchMessageCounter = 0;
    constexpr int FETCH_DATA_INTERVAL = 100; // ~5000ms (100 * 50ms)
    constexpr int FETCH_MESSAGE_INTERVAL = 5; // ~250ms (5 * 50ms)

    std::thread pollingThread([&] {
        std::unique_lock lock(mtx);
        while (running) {
            if (activeTab == 2 && !friendList.empty()) {
                fetchMessageCounter++;

                if (fetchMessageCounter >= FETCH_MESSAGE_INTERVAL) {
                    std::string selectedFriend = friendList[selectedFriendIndex];
                    (void) session.getPlayerMessages(selectedFriend);
                    fetchMessageCounter = 0;
                }

                if (selectedFriendIndex != previousFriendIndex) {
                    std::string selectedFriend = friendList[selectedFriendIndex];
                    (void) session.getPlayerMessages(selectedFriend);
                    previousFriendIndex = selectedFriendIndex;
                }
            } else {
                fetchDataCounter++;
                if (fetchDataCounter >= FETCH_DATA_INTERVAL) {
                    (void) session.fetchPlayerData();
                    fetchDataCounter = 0;
                }
            }

            screen.PostEvent(Event::Custom);
            cv.wait_for(lock, std::chrono::milliseconds(50));
        }
    });

    // Main loop
    screen.Loop(renderer);

    // Cleanup polling thread
    running = false;
    cv.notify_all();
    if (pollingThread.joinable()) {
        pollingThread.join();
    }
}
