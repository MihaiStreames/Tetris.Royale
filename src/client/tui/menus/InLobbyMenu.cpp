#include "InLobbyMenu.hpp"



std::string
gameDescription(const GameMode& gameMode)
{
    return GAMEMODE_DESCRIPTIONS.at(gameMode);
}

void
inLobbyMenu(ClientSession& clientSession)
{
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    // we get the current lobby state
    LobbyState state = clientSession.getCurrentLobbyState();
    std::vector<std::string> friendList = clientSession.getFriendList();

    // Title
    auto titlebox = Renderer(
        [&]
        { return hbox({text(GAME_TITLE) | bold | color(Color::Green1)}); });

    auto LeaveButton = Button("Leave Lobby",
                              [&screen, &clientSession]
                              {
                                    clientSession.leaveLobby();
                                    currMenu = MenuState::mainMenu;
                                    screen.Exit();
                              });

    auto ReadyButton = Button("Ready",
                              [&clientSession]
                              {
                                    clientSession.readyUp();
                              });

    auto UnreadyButton = Button("Unready",
                              [&clientSession]
                              {
                                    clientSession.unready();
                              });

    auto leftMenuButtonsContainer = Container::Vertical(
        {LeaveButton, ReadyButton, UnreadyButton});


    auto GameModeBox = Renderer(
        leftMenuButtonsContainer,
        [&]
        {
            return vbox({text("Current Game Mode : " + gameDescription(state.gameMode)) | color(Color::Green1)});
        });

    auto PlayersBox = Renderer(
        [&]
        {
            Elements lines;
            for (const auto& player : state.players)
            {
                lines.push_back(text(player.first + " : " + player.second));
            }
            return window(text("Players"), vbox(std::move(lines)));
        });

    auto SpectatorsBox = Renderer(
        [&]
        {
            Elements lines;
            for (auto& spectator : state.spectators)
            {
                lines.push_back(text(spectator));
            }
            return window(text("Spectators"), vbox(std::move(lines)));
        });

    auto leftMenuRenderer = Renderer(
        leftMenuButtonsContainer,
        [&]
        {
            return window(
                       text("Lobby : " + state.lobbyID) | color(Color::Green1),
                       vbox({GameModeBox->Render() | color(Color::Green1),
                             PlayersBox->Render() | color(Color::Green1),
                             SpectatorsBox->Render() | color(Color::Green1),
                             
                             hbox({ReadyButton->Render() | color(Color::Green1),
                                      UnreadyButton->Render() | color(Color::Green1),
                                   LeaveButton->Render() |
                                       color(Color::Green1)})})) |
                   color(Color::Green1);
        });

    // Friend list / chat
    int currentFriendPage = 0;
    const int friendsPerPage = 5;
    int selectedFriendIndex = 0;

    // Input buffer for new messages
    std::string messageInputBuffer;
    auto messageInput = Input(&messageInputBuffer, "Type your message...");

    auto sendButton =
        Button("Send",
               [&]
               {
                   if (!messageInputBuffer.empty() && !testData.friendList.empty())
                   {
                       
                        // TODO: send message to server
                       messageInputBuffer.clear();
                   }
               });

    // Pagination
    auto prevFriendButton = Button("Prev",
                                   [&]
                                   {
                                       // TODO
                                   });

    auto nextFriendButton =
        Button("Next",
               [&]
               {
                   // TODO
               });

    // Friend selector

    auto friendSelector = Menu(&friendList, &selectedFriendIndex);

    auto friendsRenderer = Renderer(
        [&]() -> Element
        {
            Elements lines;
            int start = currentFriendPage * friendsPerPage;
            int end = std::min(start + friendsPerPage,
                               static_cast<int>(friendList.size()));

            for (int i = start; i < end; i++)
            {
                lines.push_back(
                    text((i == selectedFriendIndex ? "-> " : "   ") +
                        friendList[i]));
            }

            return window(
                       text("Friends"),
                       vbox({vbox(std::move(lines)), separator(),
                             hbox({prevFriendButton->Render(),
                                   text(" Page " +
                                        std::to_string(currentFriendPage + 1) +
                                        " "),
                                   nextFriendButton->Render()})})) |
                   color(Color::Green1);
        });

    auto messagesRenderer = Renderer(
        [&]() -> Element
        {
            if (friendList.empty()) {
                return window(text("Messages"), text("No friends available.") | color(Color::Red));
            }

            std::string selectedFriend = friendList[selectedFriendIndex];
            Elements lines;
            // for (auto& msg : testData.conversations[selectedFriend])
            // {
            //    lines.push_back(text(msg.from + ": " + msg.text));
            // }
            return vbox({window(text("Messages with " + selectedFriend) |
                                    color(Color::Green1),
                                vbox(std::move(lines)) | color(Color::Green1)) |
                             color(Color::Green1),
                         separator(),
                         hbox({messageInput->Render() | size(WIDTH, EQUAL, 30) |
                                   color(Color::Green1),
                               sendButton->Render() | color(Color::Green1)})}) |
                   color(Color::Green1);
        });

    // Add friend (placeholder)
    std::string friendName; // local buffer
    auto friendInput = Input(&friendName, "Enter username...");
    auto addFriendButton = Button("Add",
                                  [&]
                                  {
                                      // e.g.
                                      // testData.friendList.push_back(friendName);
                                      // friendName.clear();
                                  });

    auto friendAddRenderer = Renderer(
        [&]() -> Element
        {
            Elements row;
            row.push_back(friendInput->Render() | size(WIDTH, EQUAL, 20));
            row.push_back(addFriendButton->Render() | color(Color::Green1));
            return window(text("Add Friend") | color(Color::Green1),
                          hbox(std::move(row))) |
                   color(Color::Green1);
        });

    auto rightContainer = Container::Vertical(
        {friendSelector, prevFriendButton, nextFriendButton, messageInput,
         sendButton, friendInput, addFriendButton});

    auto rightSideRenderer = Renderer(
        rightContainer,
        [&]() -> Element
        {
            return vbox({friendsRenderer->Render(), messagesRenderer->Render(),
                         friendAddRenderer->Render()});
        });

    auto mainContainer =
        Container::Horizontal({leftMenuButtonsContainer, rightContainer});

    auto mainBox = Renderer(
        mainContainer,
        [&]() -> Element
        {
            Elements row;
            row.push_back(leftMenuRenderer->Render() | size(WIDTH, EQUAL, 100));
            row.push_back(rightSideRenderer->Render() |
                          size(WIDTH, EQUAL, 100));
            auto mainRow = hbox(std::move(row));
            Elements col;
            col.push_back(titlebox->Render());
            col.push_back(separator() | color(Color::Green1));
            col.push_back(mainRow);
            return vbox(std::move(col));
        });

    screen.Loop(mainBox);
};
