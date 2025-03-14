#include "InLobbyScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

std::string getGameModeName(const GameMode mode) {
    switch (mode) {
        case GameMode::CLASSIC: return "Classic";
        case GameMode::DUEL:    return "Duel";
        case GameMode::ROYALE:  return "Royale";
        default:                return "Unknown";
    }
}

void showInLobbyScreen(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Initialize with an empty state
    LobbyState lobbyState = LobbyState::generateEmptyState();
    std::string errorMessage;
    bool gameStarting = false;

    // Initial fetch
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
    const auto container = Container::Vertical({
        readyButton,
        unreadyButton,
        leaveButton
    });

    // Main renderer
    const auto renderer = Renderer(container, [&] {
        // If game is starting, show countdown
        if (gameStarting) {
            return text("Game starting...") | center | bold;
        }

        // Build player list
        Elements playerElements;
        if (lobbyState.players.empty()) {
            playerElements.push_back(text("No players in lobby"));
        } else {
            for (const auto &[token, username]: lobbyState.players) {
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

    // Polling thread for lobby state
    std::atomic_bool running{true};
    std::mutex mtx;
    std::condition_variable cv;

    std::thread pollingThread([&] {
        std::unique_lock lock(mtx);
        while (running) {
            // Check if the game has started
            ClientStatus currentStatus = session.getOwnStatus();
            if (currentStatus == ClientStatus::IN_GAME) {
                gameStarting = true;
                currentScreen = ScreenState::InGame;
                screen.Exit();
                break;
            }

            // Update lobby state periodically
            try {
                LobbyState newState = session.getCurrentLobbyState();
                if (newState.lobbyID.empty()) {
                    errorMessage = "Warning: Received empty lobby state";
                } else {
                    lobbyState = newState;
                    errorMessage = "";  // Clear error on success

                    // Check if all players are ready
                    bool allReady = true;
                    int playerCount = 0;

                    for (const auto &[token, isReady] : lobbyState.readyPlayers) {
                        if (!isReady) {
                            allReady = false;
                        }
                        playerCount++;
                    }
                    if (allReady && playerCount >= 2) {
                        gameStarting = true;
                    }
                }
            } catch (const std::exception &e) {
                // In case of exception, verify if game has started.
                ClientStatus statusCheck = session.getOwnStatus();
                if (statusCheck == ClientStatus::IN_GAME) {
                    gameStarting = true;
                    currentScreen = ScreenState::InGame;
                    screen.Exit();
                    break;
                }
                errorMessage = "Error refreshing lobby: " + std::string(e.what());
            }
            // Trigger a redraw.
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