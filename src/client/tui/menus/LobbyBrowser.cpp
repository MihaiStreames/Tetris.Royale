#include "LobbyBrowser.hpp"
#include "Common.hpp"

using namespace ftxui;

// Helper to parse lobby info from server response
std::pair<std::vector<std::string>, std::vector<LobbyState> >
parseLobbies(const std::unordered_map<std::string, std::string> &lobbyData) {
    std::vector<std::string> lobbyIds;
    std::vector<LobbyState> lobbyStates;

    for (const auto &[idx, data]: lobbyData) {
        try {
            LobbyState lobbyState = LobbyState::deserialize(data);
            // Convert index to lobby ID
            lobbyIds.push_back(lobbyState.lobbyID);
            // Deserialize the lobby state
            lobbyStates.push_back(lobbyState);
        } catch (const std::exception &e) {
            // Skip invalid lobbies
            continue;
        }
    }

    return {lobbyIds, lobbyStates};
}

void showLobbyBrowser(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Error message for display
    std::string errorMessage;

    // Initial fetch of public lobbies
    auto lobbiesData = session.getPublicLobbiesList();
    auto [lobbyIds, lobbyStates] = parseLobbies(lobbiesData);

    // Create lobby selection options
    std::vector<std::string> lobbyOptions;
    if (lobbyStates.empty()) {
        lobbyOptions.push_back("No available lobbies");
    } else {
        for (const auto &state: lobbyStates) {
            std::string gameModeName;

            switch (state.gameMode) {
                case GameMode::CLASSIC: gameModeName = "Classic";
                    break;
                case GameMode::DUEL: gameModeName = "Duel";
                    break;
                case GameMode::ROYALE: gameModeName = "Royale";
                    break;
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

    // Use MenuOption to make selection more visible
    MenuOption menuOption;
    menuOption.on_enter = [&] {
        // Handle Enter key as join lobby
        if (!lobbyIds.empty() && selectedLobby < static_cast<int>(lobbyIds.size())) {
            StatusCode result = session.joinLobby(lobbyIds[selectedLobby]);
            if (result == StatusCode::SUCCESS) {
                currentScreen = ScreenState::InLobby;
                screen.Exit();
            } else {
                errorMessage = "Failed to join lobby: " + getStatusCodeString(result);
            }
        }
    };
    auto lobbyMenu = Menu(&lobbyOptions, &selectedLobby, menuOption);

    std::string lobbyCode;
    auto lobbyCodeInput = Input(&lobbyCode, "Lobby Code");

    auto joinByCodeButton = Button("Join by Code", [&] {
        if (!lobbyCode.empty()) {
            // Assumes session.joinLobby() can also use a lobby code.
            StatusCode result = session.joinLobby(lobbyCode);
            if (result == StatusCode::SUCCESS) {
                currentScreen = ScreenState::InLobby;
                screen.Exit();
            } else {
                errorMessage = "Failed to join lobby by code: " + getStatusCodeString(result);
            }
        }
    });

    auto spectateByCodeButton = Button("Spectate by Code", [&] {
        if (!lobbyCode.empty()) {
            StatusCode result = session.spectateLobby(lobbyCode);
            if (result == StatusCode::SUCCESS) {
                currentScreen = ScreenState::InGame;
                screen.Exit();
            } else {
                errorMessage = "Failed to spectate lobby by code: " + getStatusCodeString(result);
            }
        }
    });

    // Game mode selection for creating a lobby
    int selectedGameMode = 0;
    std::vector<std::string> gameModes = {"Classic", "Duel", "Royale"};
    auto gameModeToggle = Toggle(&gameModes, &selectedGameMode);

    // Lobby size slider
    int initPlayers = MIN_LOBBY_SIZE;
    auto maxPlayersInput = Slider("Max Players: ", &initPlayers, MIN_LOBBY_SIZE, MAX_LOBBY_SIZE, 1);

    // Public/Private toggle
    bool isPublic = true;
    auto publicToggle = Checkbox("Public Lobby", &isPublic);

    // Buttons
    auto joinButton = Button("Join Selected Lobby", [&] {
        // Check if there are any lobbies
        if (lobbyIds.empty() ||
            (lobbyOptions.size() == 1 && lobbyOptions[0] == "No available lobbies")) {
            errorMessage = "No lobbies available to join";
            return;
        }

        // Check if selected index is valid
        if (selectedLobby < 0 || selectedLobby >= static_cast<int>(lobbyIds.size())) {
            errorMessage = "Invalid lobby selection";
            return;
        }

        StatusCode result = session.joinLobby(lobbyIds[selectedLobby]);
        if (result == StatusCode::SUCCESS) {
            currentScreen = ScreenState::InLobby;
            screen.Exit();
        } else {
            errorMessage = "Failed to join lobby: " + getStatusCodeString(result);
        }
    });

    auto spectateButton = Button("Spectate", [&] {
        // Check if there are any lobbies
        if (lobbyIds.empty() ||
            (lobbyOptions.size() == 1 && lobbyOptions[0] == "No available lobbies")) {
            errorMessage = "No lobbies available to spectate";
            return;
        }

        // Check if selected index is valid
        if (selectedLobby < 0 || selectedLobby >= static_cast<int>(lobbyIds.size())) {
            errorMessage = "Invalid lobby selection";
            return;
        }

        StatusCode result = session.spectateLobby(lobbyIds[selectedLobby]);
        if (result == StatusCode::SUCCESS) {
            currentScreen = ScreenState::InGame;
            screen.Exit();
        } else {
            errorMessage = "Failed to spectate lobby: " + getStatusCodeString(result);
        }
    });

    auto createButton = Button("Create Lobby", [&] {
        // Check if token is empty and warn
        if (session.getToken().empty()) {
            errorMessage = "Error: Not logged in or session token missing";
            return;
        }

        GameMode mode;
        switch (selectedGameMode) {
            case 0: mode = GameMode::CLASSIC;
                break;
            case 1: mode = GameMode::DUEL;
                break;
            case 2: mode = GameMode::ROYALE;
                break;
            default: mode = GameMode::CLASSIC;
        }

        StatusCode result = session.createAndJoinLobby(mode, initPlayers, isPublic);

        if (result == StatusCode::SUCCESS) {
            // Successfully created and joined the lobby
            currentScreen = ScreenState::InLobby;
            screen.Exit();
        } else {
            // Display error message
            errorMessage = "Failed to create lobby: " + getStatusCodeString(result);
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
                for (const auto &state: lobbyStates) {
                    std::string gameModeName;

                    switch (state.gameMode) {
                        case GameMode::CLASSIC: gameModeName = "Classic";
                            break;
                        case GameMode::DUEL: gameModeName = "Duel";
                            break;
                        case GameMode::ROYALE: gameModeName = "Royale";
                            break;
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
                selectedLobby = lobbyOptions.empty() ? 0 : static_cast<int>(lobbyOptions.size()) - 1;
            }

            // Clear any error message on successful refresh
            errorMessage = "";
        } catch (const std::exception &e) {
            errorMessage = "Error refreshing lobbies: ";
            errorMessage += e.what();
        }
    });

    auto backButton = Button("Back", [&] {
        currentScreen = ScreenState::MainMenu;
        screen.Exit();
    });

    auto container = Container::Vertical({
        lobbyMenu,
        gameModeToggle,
        maxPlayersInput,
        publicToggle,
        joinButton,
        spectateButton,
        lobbyCodeInput,
        joinByCodeButton,
        spectateByCodeButton,
        createButton,
        refreshButton,
        backButton
    });

    container->SetActiveChild(nullptr);

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

        // Add error message if present
        Elements mainContent;
        mainContent.push_back(text("LOBBY BROWSER") | bold | center);

        if (!errorMessage.empty()) {
            mainContent.push_back(text(errorMessage) | color(Color::Red));
        }

        mainContent.push_back(separator());
        mainContent.push_back(text("Available Lobbies:") | bold);
        mainContent.push_back(lobbyMenu->Render() | border | size(HEIGHT, GREATER_THAN, 10) | flex);

        mainContent.push_back(hbox({
            joinButton->Render(),
            spectateButton->Render(),
            refreshButton->Render(),
            lobbyCodeInput->Render(),
            joinByCodeButton->Render(),
            spectateByCodeButton->Render()
        }));
        mainContent.push_back(separator());
        mainContent.push_back(createLobbyPanel);
        mainContent.push_back(separator());
        mainContent.push_back(backButton->Render() | center);

        return vbox(mainContent) | border | color(Color::Green);
    });

    // Polling thread to refresh lobbies
    std::atomic_bool running{true};
    std::mutex mtx;
    std::condition_variable cv;

    int fetchLobbyCounter = 0;
    constexpr int FETCH_LOBBIES_INTERVAL = 100; // ~5 seconds if using 50ms intervals

    std::thread pollingThread([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        while (running) {
            fetchLobbyCounter++;
            if (fetchLobbyCounter >= FETCH_LOBBIES_INTERVAL) {
                // Refresh public lobbies.
                lobbiesData = session.getPublicLobbiesList();
                auto result = parseLobbies(lobbiesData);

                lobbyIds = result.first;
                lobbyStates = result.second;

                lobbyOptions.clear();
                if (lobbyStates.empty()) {
                    lobbyOptions.push_back("No available lobbies");
                } else {
                    for (const auto &state : lobbyStates) {
                        std::string gameModeName;
                        switch (state.gameMode) {
                            case GameMode::CLASSIC: gameModeName = "Classic";   break;
                            case GameMode::DUEL:    gameModeName = "Duel";      break;
                            case GameMode::ROYALE:  gameModeName = "Royale";    break;
                            default:                gameModeName = "Unknown";
                        }
                        std::stringstream ss;
                        ss << "ID: " << state.lobbyID
                           << " | Mode: " << gameModeName
                           << " | Players: " << state.players.size() << "/" << state.maxPlayers
                           << " | Spectators: " << state.spectators.size();
                        lobbyOptions.push_back(ss.str());
                    }
                }
                if (selectedLobby >= static_cast<int>(lobbyOptions.size())) {
                    selectedLobby = lobbyOptions.empty() ? 0 : static_cast<int>(lobbyOptions.size()) - 1;
                }
                fetchLobbyCounter = 0;
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
