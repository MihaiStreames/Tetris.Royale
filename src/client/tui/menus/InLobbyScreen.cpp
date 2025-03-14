#include "InLobbyScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

std::string getGameModeName(GameMode mode) {
    switch (mode) {
        case GameMode::CLASSIC: return "Classic";
        case GameMode::DUEL: return "Duel";
        case GameMode::ROYALE: return "Royale";
        default: return "Unknown";
    }
}

void showInLobbyScreen(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Initialize with an empty state
    LobbyState lobbyState = LobbyState::generateEmptyState();
    std::string errorMessage;

    // Initial fetch with error handling
    try {
        lobbyState = session.getCurrentLobbyState();

        // If we got an empty lobby ID, there might be a problem
        if (lobbyState.lobbyID.empty()) {
            errorMessage = "Warning: Received empty lobby state";
        }
    } catch (const std::exception &e) {
        errorMessage = "Error getting initial lobby state: ";
        errorMessage += e.what();
    }

    // Create buttons
    auto readyButton = Button("Ready", [&] {
        if (session.readyUp() != StatusCode::SUCCESS) {
            errorMessage = "Error setting ready status";
        }
    });

    auto unreadyButton = Button("Unready", [&] {
        if (session.unreadyUp() != StatusCode::SUCCESS) {
            errorMessage = "Error setting unready status";
        }
    });

    auto leaveButton = Button("Leave Lobby", [&] {
        try {

            if (session.leaveLobby() == StatusCode::SUCCESS) {
                currentScreen = ScreenState::LobbyBrowser;
                screen.Exit();
            } else {
                errorMessage = "Error leaving lobby";
            }
        
        } catch (const std::exception &e) {

            errorMessage = "Error leaving lobby: ";
            errorMessage += e.what();

        }
    });

    // Container for interactive components
    auto container = Container::Vertical({
        readyButton,
        unreadyButton,
        leaveButton
    });

    // Variable to track refresh cycle
    int refreshCounter = 0;

    // Main renderer with auto-refresh
    auto renderer = Renderer(container, [&] {
        // Refresh the lobby state periodically
        if (refreshCounter++ % 10 == 0) {
            try {
                LobbyState newState = session.getCurrentLobbyState();

                // Check if we got a valid state
                if (newState.lobbyID.empty()) {
                    errorMessage = "Warning: Received empty lobby state";
                } else {
                    lobbyState = newState;
                    errorMessage = ""; // Clear error on successful update

                    // Check if the game is starting - all players ready
                    bool allReady = true;
                    int playerCount = 0;

                    for (const auto &[token, isReady]: lobbyState.readyPlayers) {
                        if (!isReady) {
                            allReady = false;
                        }
                        playerCount++;
                    }

                    // Only transition if we have enough ready players
                    if (allReady && playerCount >= 2) {
                        currentScreen = ScreenState::InGame;
                        screen.Exit();
                        return text("Starting game...") | center;
                    }
                }
            } catch (const std::exception &e) {
                errorMessage = "Error refreshing lobby: ";
                errorMessage += e.what();
                // Keep the existing state
            }
        }

        // Build player list with safe iteration
        Elements playerElements;
        if (lobbyState.players.empty()) {
            playerElements.push_back(text("No players in lobby"));
        } else {
            for (const auto &[token, username]: lobbyState.players) {
                // Safe lookup in readyPlayers
                bool isReady = false;
                auto it = lobbyState.readyPlayers.find(token);
                if (it != lobbyState.readyPlayers.end()) {
                    isReady = it->second;
                }

                std::string statusStr = isReady ? " [Ready]" : " [Not Ready]";
                playerElements.push_back(text(username + statusStr));
            }
        }

        // Build spectator list with safe iteration
        Elements spectatorElements;
        if (lobbyState.spectators.empty()) {
            spectatorElements.push_back(text("No spectators"));
        } else {
            for (const auto &[token, username]: lobbyState.spectators) {
                spectatorElements.push_back(text(username));
            }
        }

        // Main content elements
        Elements content;
        content.push_back(text("LOBBY: " + lobbyState.lobbyID) | bold | center);
        content.push_back(text("Game Mode: " + getGameModeName(lobbyState.gameMode)));

        // Display error message if any
        if (!errorMessage.empty()) {
            content.push_back(text(errorMessage) | color(Color::Red));
        }

        content.push_back(separator());
        content.push_back(text("Players: (" + std::to_string(lobbyState.players.size()) + "/" +
                               std::to_string(lobbyState.maxPlayers) + ")") | bold);
        content.push_back(vbox(playerElements) | border);
        content.push_back(text("Spectators: " + std::to_string(lobbyState.spectators.size())) | bold);
        content.push_back(vbox(spectatorElements) | border);
        content.push_back(separator());
        content.push_back(hbox({
            readyButton->Render(),
            unreadyButton->Render(),
            leaveButton->Render()
        }));

        return vbox(content) | border | color(Color::Green);
    });

    // Main loop
    screen.Loop(renderer);
}
