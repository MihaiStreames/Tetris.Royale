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

void showInLobbyScreen(ClientSession& session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Get initial lobby state
    LobbyState lobbyState = session.getCurrentLobbyState();

    // Create buttons
    auto readyButton = Button("Ready", [&] {
        session.readyUp();
    });

    auto unreadyButton = Button("Unready", [&] {
        session.unreadyUp();
    });

    auto leaveButton = Button("Leave Lobby", [&] {
        session.leaveLobby();
        currentScreen = ScreenState::LobbyBrowser;
        screen.Exit();
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

                // Check if the game is starting - all players ready
                bool allReady = true;
                for (const auto& [token, isReady] : newState.readyPlayers) {
                    if (!isReady) {
                        allReady = false;
                        break;
                    }
                }

                // If all players are ready and there are enough players, redirect to game
                if (allReady && newState.players.size() >= 2) {
                    currentScreen = ScreenState::InGame;
                    screen.Exit();
                    return text("Starting game...") | center;
                }

                lobbyState = newState;
            } catch (const std::exception& e) {
                // Handle error - possibly lost connection
            }
        }

        // Display players with ready status
        Elements playerElements;
        for (const auto& [token, username] : lobbyState.players) {
            bool isReady = lobbyState.readyPlayers.contains(token) && lobbyState.readyPlayers.at(token);
            std::string statusStr = isReady ? " [Ready]" : " [Not Ready]";
            playerElements.push_back(text(username + statusStr));
        }

        // Display spectators
        Elements spectatorElements;
        for (const auto& [token, username] : lobbyState.spectators) {
            spectatorElements.push_back(text(username));
        }

        return vbox({
            text("LOBBY: " + lobbyState.lobbyID) | bold | center,
            text("Game Mode: " + getGameModeName(lobbyState.gameMode)),
            separator(),
            text("Players: (" + std::to_string(lobbyState.players.size()) + "/" +
                 std::to_string(lobbyState.maxPlayers) + ")") | bold,
            vbox(playerElements) | border,
            text("Spectators: " + std::to_string(lobbyState.spectators.size())) | bold,
            vbox(spectatorElements) | border,
            separator(),
            hbox({
                readyButton->Render(),
                unreadyButton->Render(),
                leaveButton->Render()
            })
        }) | border | color(Color::Green);
    });

    // Main loop
    screen.Loop(renderer);
}