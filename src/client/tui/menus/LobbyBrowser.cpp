#include "LobbyBrowser.hpp"
#include "Common.hpp"
#include <sstream>

using namespace ftxui;

// Helper to parse lobby info from server response
std::pair<std::vector<std::string>, std::vector<LobbyState>>
parseLobbies(const std::unordered_map<std::string, std::string>& lobbyData) {
    std::vector<std::string> lobbyIds;
    std::vector<LobbyState> lobbyStates;

    for (const auto& [id, data] : lobbyData) {
        try {
            // Convert index to lobby ID
            lobbyIds.push_back(id);
            // Deserialize the lobby state
            lobbyStates.push_back(LobbyState::deserialize(data));
        } catch (const std::exception& e) {
            // Skip invalid lobbies
            continue;
        }
    }

    return {lobbyIds, lobbyStates};
}

void showLobbyBrowser(ClientSession& session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Get list of public lobbies
    auto lobbiesData = session.getPublicLobbiesList();
    auto [lobbyIds, lobbyStates] = parseLobbies(lobbiesData);

    // Create lobby selection options
    std::vector<std::string> lobbyOptions;
    if (lobbyStates.empty()) {
        lobbyOptions.push_back("No available lobbies");
    } else {
        for (const auto& state : lobbyStates) {
            std::string gameModeName;

            switch (state.gameMode) {
                case GameMode::CLASSIC: gameModeName = "Classic"; break;
                case GameMode::DUEL: gameModeName = "Duel"; break;
                case GameMode::ROYALE: gameModeName = "Royale"; break;
                default: gameModeName = "Unknown";
            }

            std::stringstream ss;
            ss << "ID: " << state.lobbyID
               << " | Mode: " << gameModeName
               << " | Players: " << state.players.size() << "/" << state.maxPlayers
               << " | Spectators: " << state.spectators.size();

            lobbyOptions.push_back(ss.str());
        }
    }

    // Selected lobby index
    int selectedLobby = 0;
    auto lobbyMenu = Menu(&lobbyOptions, &selectedLobby);

    // Game mode selection for creating a lobby
    int selectedGameMode = 0;
    std::vector<std::string> gameModes = {"Classic", "Duel", "Royale"};
    auto gameModeToggle = Toggle(&gameModes, &selectedGameMode);

    // Max players input for creating a lobby
    int maxPlayers = 4;
    auto maxPlayersInput = Slider("Max Players: ", &maxPlayers, 2, 8, 1);

    // Public/Private toggle
    bool isPublic = true;
    auto publicToggle = Checkbox("Public Lobby", &isPublic);

    // Buttons
    auto joinButton = Button("Join Selected Lobby", [&] {
        if (!lobbyIds.empty() && selectedLobby < static_cast<int>(lobbyIds.size())) {
            try {
                session.joinLobby(lobbyIds[selectedLobby]);
                currentScreen = ScreenState::InLobby;
                screen.Exit();
            } catch (const std::exception& e) {
                // Handle join error
            }
        }
    });

    auto spectateButton = Button("Spectate", [&] {
        if (!lobbyIds.empty() && selectedLobby < static_cast<int>(lobbyIds.size())) {
            try {
                session.spectateLobby(lobbyIds[selectedLobby]);
                currentScreen = ScreenState::InGame;
                screen.Exit();
            } catch (const std::exception& e) {
                // Handle spectate error
            }
        }
    });

    auto createButton = Button("Create Lobby", [&] {
        try {
            GameMode mode;
            switch (selectedGameMode) {
                case 0: mode = GameMode::CLASSIC; break;
                case 1: mode = GameMode::DUEL; break;
                case 2: mode = GameMode::ROYALE; break;
                default: mode = GameMode::CLASSIC;
            }

            // Make sure max players is within valid range
            if (maxPlayers < 2) maxPlayers = 2;
            if (maxPlayers > 8) maxPlayers = 8;

            // Simply call the session's method and immediately transition
            // The ClientSession::createAndJoinLobby should handle both
            // creating the lobby and joining it in one operation
            session.createAndJoinLobby(mode, maxPlayers, isPublic);

            // Immediately transition to in-lobby screen without checking anything
            currentScreen = ScreenState::InLobby;
            screen.Exit();
        } catch (...) {
            // Any error, just ignore and stay on the current screen
        }
    });

    auto refreshButton = Button("Refresh", [&] {
        try {
            // Update the lobby list
            lobbiesData = session.getPublicLobbiesList();
            auto result = parseLobbies(lobbiesData);
            lobbyIds = result.first;
            lobbyStates = result.second;

            // Rebuild options
            lobbyOptions.clear();
            if (lobbyStates.empty()) {
                lobbyOptions.push_back("No available lobbies");
            } else {
                for (const auto& state : lobbyStates) {
                    std::string gameModeName;

                    switch (state.gameMode) {
                        case GameMode::CLASSIC: gameModeName = "Classic"; break;
                        case GameMode::DUEL: gameModeName = "Duel"; break;
                        case GameMode::ROYALE: gameModeName = "Royale"; break;
                        default: gameModeName = "Unknown";
                    }

                    std::stringstream ss;
                    ss << "ID: " << state.lobbyID
                       << " | Mode: " << gameModeName
                       << " | Players: " << state.players.size() << "/" << state.maxPlayers
                       << " | Spectators: " << state.spectators.size();

                    lobbyOptions.push_back(ss.str());
                }
            }

            // Reset selection if needed
            if (selectedLobby >= static_cast<int>(lobbyOptions.size())) {
                selectedLobby = lobbyOptions.empty() ? 0 : lobbyOptions.size() - 1;
            }
        } catch (const std::exception& e) {
            // Handle refresh error
        }
    });

    auto backButton = Button("Back", [&] {
        currentScreen = ScreenState::MainMenu;
        screen.Exit();
    });

    // Container for interactive components - IMPORTANT: add menu component here ONCE
    auto container = Container::Vertical({
        lobbyMenu,      // Add the menu ONCE here
        gameModeToggle,
        maxPlayersInput,
        publicToggle,
        joinButton,
        spectateButton,
        createButton,
        refreshButton,
        backButton
    });

    // Main renderer
    auto renderer = Renderer(container, [&] {
        // Create lobby panel
        auto createLobbyPanel = vbox({
            text("Create New Lobby") | bold | center,
            gameModeToggle->Render(),
            maxPlayersInput->Render(),
            publicToggle->Render(),
            createButton->Render() | center
        }) | border;

        return vbox({
            text("LOBBY BROWSER") | bold | center,
            separator(),
            text("Available Lobbies:") | bold,
            lobbyMenu->Render() | border | size(HEIGHT, GREATER_THAN, 10) | flex,
            hbox({
                joinButton->Render(),
                spectateButton->Render(),
                refreshButton->Render()
            }),
            separator(),
            createLobbyPanel,
            separator(),
            backButton->Render() | center
        }) | border | color(Color::Green);
    });

    // Main loop
    screen.Loop(renderer);
}