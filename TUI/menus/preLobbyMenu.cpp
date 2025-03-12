#include "preLobbyMenu.hpp"
 

using namespace ftxui;

// Très crade mais ça marche -> à améliorer
void preLobbyMenu() {
    auto screen = ScreenInteractive::Fullscreen();

    //auto data = TestData();
    auto& data = testData;

    // Title
    auto titlebox = Renderer([&] {
        return hbox({
            text(data.gameTitle) | bold | color(Color::Green1)
        });
    });

    auto SoloButton = Button("Solo Mode",[]{
        //Lancer endless
        //currMenu = MenuState::inGameMenu();
    });

    auto CreateButton = Button("Create Lobby",[&screen]{
        //Créer un lobby
        currMenu = MenuState::inLobbyMenu;
        screen.Exit();
    });

    auto LeaveButton = Button("Back to Main Menu",[]{
        //Quitter le preLobby
        //currMenu = MenuState::mainMenu();
    });


   auto PublicLobbiesBox = Renderer([&] {
    Elements lines;
    for (auto& lobby : data.lobbies) {
        lines.push_back(hbox({
            text(lobby.lobbyID + " - Mode: " 
                 + lobby.gameMode + " - Players: " 
                 + std::to_string(lobby.nbPlayers) + " - Spectators: " 
                 + std::to_string(lobby.nbSpectators) + " - Status: "
                 + lobby.status) | color(Color::Green1) | border
        }));
    }

    return window(text("Public Lobbies"), vbox(std::move(lines))) | color(Color::Green1);
});

    auto leftMenuButtonsContainer = Container::Vertical({
        SoloButton,
        CreateButton,
        LeaveButton
    });


    auto leftMenuRenderer = Renderer(leftMenuButtonsContainer, [&]{
        return vbox({
            SoloButton->Render() | color(Color::Green1),
            PublicLobbiesBox->Render() | color(Color::Green1),
            CreateButton->Render() | color(Color::Green1),
            LeaveButton->Render() | color(Color::Green1)
        });
    });
    
    
    // Friend list / chat
    int currentFriendPage = 0;
    const int friendsPerPage = 5;
    int selectedFriendIndex = 0;

    // Input buffer for new messages
    std::string messageInputBuffer;
    auto messageInput = Input(&messageInputBuffer, "Type your message...") | border | color(Color::Green1);

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
        })) | color(Color::Green1);
    });

    auto messagesRenderer = Renderer([&]() -> Element {
    if (data.friendList.empty()) {
        return window(text("Messages"), text("No friends available.") | color(Color::Red));
    }

    std::string selectedFriend = data.friendList[selectedFriendIndex]; 
    Elements lines;
    for (auto& msg : data.conversations[selectedFriend]) {
        lines.push_back(text(msg.from + ": " + msg.text));
    }
    return vbox({
                window(text("Messages with " + selectedFriend) | color(Color::Green1),
                vbox(std::move(lines))) | color(Color::Green1),
                hbox({ messageInput->Render() | size(WIDTH, EQUAL, 30) | color(Color::Green1), 
                        sendButton->Render() | color(Color::Green1)
                    })
            }) | color(Color::Green1);
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
        return window(text("Add Friend") | color(Color::Green1), hbox(std::move(row))) | color(Color::Green1);
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
        leftMenuButtonsContainer,
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
};
