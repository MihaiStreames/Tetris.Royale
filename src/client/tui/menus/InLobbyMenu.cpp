#include "InLobbyMenu.hpp"
#include "MenuHandler.hpp"

std::string currGameMode = "Classic";

std::string
gameDescription(const GameMode& gameMode)
{
    if (currGameMode == "Classic")
    {
        return gameMode.descriptions.at("ClassicButton");
    }
    else if (currGameMode == "Duel")
    {
        return gameMode.descriptions.at("DuelButton");
    }
    else if (currGameMode == "Royale")
    {
        return gameMode.descriptions.at("RoyaleButton");
    }
    else
    {
        return "";
    }
}

void
inLobbyMenu()
{
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    auto data = TestData();
    auto gameMode = GameMode();

    // Title
    auto titlebox = Renderer(
        [&]
        { return hbox({text(data.gameTitle) | bold | color(Color::Green1)}); });

    // Menu buttons
    auto ClassicButton = Button("Classic",
                                [&]
                                {
                                    // Choisir mode de jeu classic
                                    currGameMode = "Classic";
                                });

    auto DuelButton = Button("Duel",
                             [&]
                             {
                                 // Choisir mode de jeu Duel
                                 currGameMode = "Duel";
                             });

    auto RoyaleButton = Button("Royal",
                               [&]
                               {
                                   // Choisir mode de jeu Royale
                                   currGameMode = "Royale";
                               });

    auto LeaveButton = Button("Leave Lobby",
                              [&screen]
                              {
                                  currMenu = MenuState::mainMenu;
                                  screen.Exit();
                              });

    auto ReadyButton = Button("Ready",
                              []
                              {
                                  // todo
                              });

    auto JoinPlayerButton = Button("Join as player",
                                   []
                                   {
                                       // Rejoindre lobby en tant que joueur
                                   });

    auto JoinSpectatorButton = Button("Join as spectator",
                                      []
                                      {
                                          // todo
                                      });

    auto leftMenuButtonsContainer = Container::Vertical(
        {ClassicButton, DuelButton, RoyaleButton, JoinSpectatorButton,
         JoinPlayerButton, ReadyButton, LeaveButton});

    // Demander à Roberto pour savoir commment ça marche | Commentaires +
    // liaison au serveur à faire
    auto GameModeBox = Renderer(
        leftMenuButtonsContainer,
        [&]
        {
            return vbox({text("Game Modes") | color(Color::Green1),
                         hbox({ClassicButton->Render() | color(Color::Green1),
                               DuelButton->Render() | color(Color::Green1),
                               RoyaleButton->Render() | color(Color::Green1)}),
                         text(gameDescription(gameMode)) |
                             color(Color::Green1)}) |
                   border;
        });

    auto PlayersBox = Renderer(
        [&]
        {
            Elements lines;
            for (const auto& player : data.players)
            {
                lines.push_back(text(player.first + " : " + player.second));
            }
            return window(text("Players"), vbox(std::move(lines)));
        });

    auto SpectatorsBox = Renderer(
        [&]
        {
            Elements lines;
            for (auto& spectator : data.spectators)
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
                       text("Lobby : " + data.lobbyName) | color(Color::Green1),
                       vbox({GameModeBox->Render() | color(Color::Green1),
                             PlayersBox->Render() | color(Color::Green1),
                             SpectatorsBox->Render() | color(Color::Green1),
                             hbox({JoinSpectatorButton->Render() |
                                       color(Color::Green1),
                                   JoinPlayerButton->Render() |
                                       color(Color::Green1)}),
                             hbox({ReadyButton->Render() | color(Color::Green1),
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
                   if (!messageInputBuffer.empty())
                   {
                       std::string selectedFriend =
                           data.friendList[selectedFriendIndex];
                       data.conversations[selectedFriend].push_back(
                           {"Me", messageInputBuffer});
                       messageInputBuffer.clear();
                   }
               });

    // Pagination
    auto prevFriendButton = Button("Prev",
                                   [&]
                                   {
                                       if (currentFriendPage > 0)
                                           currentFriendPage--;
                                   });

    auto nextFriendButton =
        Button("Next",
               [&]
               {
                   if ((currentFriendPage + 1) * friendsPerPage <
                       static_cast<int>(data.friendList.size()))
                       currentFriendPage++;
               });

    // Friend selector
    auto friendSelector = Menu(&data.friendList, &selectedFriendIndex);

    auto friendsRenderer = Renderer(
        [&]() -> Element
        {
            Elements lines;
            int start = currentFriendPage * friendsPerPage;
            int end = std::min(start + friendsPerPage,
                               static_cast<int>(data.friendList.size()));

            for (int i = start; i < end; i++)
            {
                lines.push_back(
                    text((i == selectedFriendIndex ? "-> " : "   ") +
                         data.friendList[i]));
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
            std::string selectedFriend = data.friendList[selectedFriendIndex];
            Elements lines;
            for (auto& msg : data.conversations[selectedFriend])
            {
                lines.push_back(text(msg.from + ": " + msg.text));
            }
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
                                      // data.friendList.push_back(friendName);
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
