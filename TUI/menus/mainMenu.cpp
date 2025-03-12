#include "mainMenu.hpp"

void mainMenu() {
    using namespace ftxui;
    auto& data = testData;
    auto screen = ScreenInteractive::Fullscreen();

    // Title
    auto titlebox = Renderer([&] {
        return hbox({
            text(data.gameTitle) | bold | color(Color::Green1)
        });
    });

    // Menu buttons
    auto playButton = Button("Play", [&screen] {
        currMenu = MenuState::preLobbyMenu;
        screen.Exit();
    });

    auto settingsButton = Button("Settings", [&screen] {
        currMenu = MenuState::settingsMenu;
        screen.Exit();
    });

    bool showLeaderboard = true;
    auto hideShowLeaderboardButton = Button("Hide/Show Leaderboard", [&] {
        showLeaderboard = !showLeaderboard;
    });

    auto logoutButton = Button("Logout", [&screen] {
        currMenu = MenuState::loginMenu;
        screen.Exit();
    });

    // Leaderboard
    auto leaderboardRenderer = Renderer([&]() -> Element {
        Elements rows;
        {
            Elements cols;
            cols.push_back(text("Rank")  | size(WIDTH, EQUAL, 10));
            cols.push_back(text("Name")  | size(WIDTH, EQUAL, 10));
            cols.push_back(text("Score") | size(WIDTH, EQUAL, 10));
            rows.push_back(hbox(std::move(cols)) | border);
        }
        for (auto& player : data.leaderboardData) {
            Elements cols;
            cols.push_back(text(std::to_string(player.rank)) | size(WIDTH, EQUAL, 10));
            cols.push_back(text(player.name)                 | size(WIDTH, EQUAL, 10));
            cols.push_back(text(std::to_string(player.score))| size(WIDTH, EQUAL, 10));
            rows.push_back(hbox(std::move(cols)) | border);
        }
        return window(text("Leaderboard") | color(Color::Green1), vbox(std::move(rows)));
    });

    // Notifications
    auto notificationsContainer = Container::Vertical({});
    std::vector<Component> notificationRows;
    std::function<void()> rebuildNotificationsUI;
    rebuildNotificationsUI = [&] {
        notificationRows.clear();
        notificationsContainer->DetachAllChildren();

        // reference the notifications from placeholder
        for (size_t i = 0; i < data.notifications.size(); i++) {
            auto acceptButton = Button("Accept", [&, i] {
                std::cout << "Accepted invite: " << data.notifications[i].message << std::endl;
                data.notifications.erase(data.notifications.begin() + i);
                rebuildNotificationsUI();
            });
            auto declineButton = Button("Decline", [&, i] {
                std::cout << "Declined invite: " << data.notifications[i].message << std::endl;
                data.notifications.erase(data.notifications.begin() + i);
                rebuildNotificationsUI();
            });
            auto notifText = Renderer([i, &data] {
                return text(data.notifications[i].message) | color(Color::Green1);
            });
            auto row = Container::Horizontal({
                notifText,
                acceptButton,
                declineButton,
            });
            notificationRows.push_back(row);
            notificationsContainer->Add(row);
        }
    };
    rebuildNotificationsUI();

    auto notificationsRenderer = Renderer(notificationsContainer, [&]() -> Element {
        Elements lines;
        for (auto& row : notificationRows) {
            lines.push_back(row->Render());
        }
        return window(text("Notifications") | color(Color::Green1), vbox(std::move(lines)));
    });

    // Left menu container
    auto leftMenuContainer = Container::Vertical({
        playButton,
        settingsButton,
        hideShowLeaderboardButton,
        logoutButton,
        notificationsContainer
    });

    auto leftMenuRenderer = Renderer(leftMenuContainer, [&]() -> Element {
        Elements btns;
        btns.push_back(playButton->Render()               | color(Color::Green1));
        btns.push_back(settingsButton->Render()           | color(Color::Green1));
        btns.push_back(hideShowLeaderboardButton->Render()| color(Color::Green1));
        btns.push_back(logoutButton->Render()             | color(Color::Green1));

        auto buttonsBox = vbox(std::move(btns));
        auto mainMenuWindow = window(text("Main Menu") | color(Color::Green1), buttonsBox);
        Elements col;
        col.push_back(mainMenuWindow);
        col.push_back(separator());
        if (showLeaderboard) {
            col.push_back(leaderboardRenderer->Render());
            col.push_back(separator());
        }
        col.push_back(notificationsRenderer->Render());
        return vbox(std::move(col));
    });

    // Friend list / chat
    int currentFriendPage = 0;
    const int friendsPerPage = 5;
    int selectedFriendIndex = 0;

    // Input buffer for new messages
    std::string messageInputBuffer;
    auto messageInput = Input(&messageInputBuffer, "Type your message...");

    auto sendButton = Button("Send", [&] {
        if (!messageInputBuffer.empty()) {
            std::string selectedFriend = data.friendList[selectedFriendIndex];
            data.conversations[selectedFriend].push_back({"Me", messageInputBuffer});
            messageInputBuffer.clear();
        }
    });

    // Pagination
    auto prevFriendButton = Button("Prev", [&] {
        if (currentFriendPage > 0) currentFriendPage--;
    });

    auto nextFriendButton = Button("Next", [&] {
        if ((currentFriendPage + 1) * friendsPerPage < static_cast<int>(data.friendList.size()))
            currentFriendPage++;
    });

    // Friend selector
    auto friendSelector = Menu(&data.friendList, &selectedFriendIndex);

    auto friendsRenderer = Renderer([&]() -> Element {
        Elements lines;
        int start = currentFriendPage * friendsPerPage;
        int end = std::min(start + friendsPerPage, static_cast<int>(data.friendList.size()));

        for (int i = start; i < end; i++) {
            lines.push_back(text((i == selectedFriendIndex ? "-> " : "   ") + data.friendList[i]));
        }

        return window(text("Friends"), vbox({
            vbox(std::move(lines)),
            separator(),
            hbox({
                prevFriendButton->Render(),
                text(" Page " + std::to_string(currentFriendPage + 1) + " "),
                nextFriendButton->Render()
            })
        }));
    });

    auto messagesRenderer = Renderer([&]() -> Element {
        std::string selectedFriend = data.friendList[selectedFriendIndex];
        Elements lines;
        for (auto& msg : data.conversations[selectedFriend]) {
            lines.push_back(text(msg.from + ": " + msg.text));
        }
        return vbox({
            window(text("Messages with " + selectedFriend) | color(Color::Green1),
                   vbox(std::move(lines))),
            separator(),
            hbox({ messageInput->Render() | size(WIDTH, EQUAL, 30), sendButton->Render() })
        });
    });

    // Add friend (placeholder)
    std::string friendName; // local buffer
    auto friendInput = Input(&friendName, "Enter username...");
    auto addFriendButton = Button("Add", [&] {
        // e.g. data.friendList.push_back(friendName);
        // friendName.clear();
    });

    auto friendAddRenderer = Renderer([&]() -> Element {
        Elements row;
        row.push_back(friendInput->Render() | size(WIDTH, EQUAL, 20));
        row.push_back(addFriendButton->Render() | color(Color::Green1));
        return window(text("Add Friend") | color(Color::Green1), hbox(std::move(row)));
    });

    auto rightContainer = Container::Vertical({
        friendSelector,
        prevFriendButton,
        nextFriendButton,
        messageInput,
        sendButton,
        friendInput,
        addFriendButton
    });

    auto rightSideRenderer = Renderer(rightContainer, [&]() -> Element {
        return vbox({
            friendsRenderer->Render(),
            messagesRenderer->Render(),
            friendAddRenderer->Render()
        });
    });

    auto mainContainer = Container::Horizontal({
        leftMenuContainer,
        rightContainer
    });

    auto mainBox = Renderer(mainContainer, [&]() -> Element {
        Elements row;
        row.push_back(leftMenuRenderer->Render() | size(WIDTH, EQUAL, 100));
        row.push_back(rightSideRenderer->Render() | size(WIDTH, EQUAL, 100));
        auto mainRow = hbox(std::move(row));
        Elements col;
        col.push_back(titlebox->Render());
        col.push_back(separator() | color(Color::Green1));
        col.push_back(mainRow);
        return vbox(std::move(col));
    });

    screen.Loop(mainBox);
}