#include "GameScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

Decorator colorForValue(const int value) {
    switch (value) {
        case 1: return color(Color::Yellow); // I
        case 2: return color(Color::LightGoldenrod1); // O
        case 3: return color(Color::Green); // J
        case 4: return color(Color::Magenta); // Z
        case 5: return color(Color::Red); // S
        case 6: return color(Color::Cyan); // T
        case 7: return color(Color::Blue); // L
        case 8: return color(Color::White); // Ghost/preview
        default: return color(Color::Default);
    }
}

// Render a tetris board
Element renderBoard(const tetroMat &board, const bool darkMode, const bool isOpponentBoard) {
    if (board.empty()) {
        return text("No board data") | center;
    }

    const int height = static_cast<int>(board.size());
    const int width = static_cast<int>(board[0].size());

    // Calculate cell size - smaller for opponent board
    const std::string cellFull = "██"; // isOpponentBoard ? "█" : "██";
    const std::string cellEmpty = "  "; // isOpponentBoard ? " " : "  ";

    Elements rows;
    for (int y = 0; y < height; ++y) {
        Elements cells;
        for (int x = 0; x < width; ++x) {
            std::string cellStr;
            if (darkMode && board[y][x] != 0) {
                cellStr = cellFull; // In dark mode, only show filled cells
            } else if (!darkMode) {
                cellStr = board[y][x] == 0 ? cellEmpty : cellFull;
            } else {
                cellStr = cellEmpty; // Empty in dark mode
            }

            cells.push_back(text(cellStr) | colorForValue(board[y][x]));
        }
        rows.push_back(hbox(cells));
    }

    return window(
        text(isOpponentBoard ? "OPPONENT" : "BOARD") | bold | color(Color::White),
        vbox(rows)
    );
}

// Render energy bar
Element renderEnergyBar(const int energy) {
    constexpr int maxEnergy = MAX_ENERGY;
    const int filledCells = energy * 10 / maxEnergy;

    Elements barElements;
    for (int i = 0; i < 10; i++) {
        if (i < filledCells) {
            barElements.push_back(text("█") | color(Color::Green));
        } else {
            barElements.push_back(text("░") | color(Color::GrayDark));
        }
    }

    return vbox({
        text("Energy: " + std::to_string(energy) + "/" + std::to_string(maxEnergy)),
        hbox(barElements)
    });
}

// Render a tetromino preview (hold/next)
Element renderPiece(PieceType type, const int height, const int width) {
    // Initialize an empty canvas grid
    std::vector canvas(height, std::vector(width, false));

    // Draw the tetromino shape based on its type
    switch (type) {
        case PieceType::I:
            if (height >= 4 && width >= 4) {
                for (int i = 0; i < 4; i++) canvas[1][i] = true;
            }
            break;
        case PieceType::O:
            if (height >= 2 && width >= 2) {
                canvas[0][0] = canvas[0][1] = canvas[1][0] = canvas[1][1] = true;
            }
            break;
        case PieceType::T:
            if (height >= 3 && width >= 3) {
                canvas[0][1] = true;
                canvas[1][0] = canvas[1][1] = canvas[1][2] = true;
            }
            break;
        case PieceType::S:
            if (height >= 3 && width >= 3) {
                canvas[0][1] = canvas[0][2] = true;
                canvas[1][0] = canvas[1][1] = true;
            }
            break;
        case PieceType::Z:
            if (height >= 3 && width >= 3) {
                canvas[0][0] = canvas[0][1] = true;
                canvas[1][1] = canvas[1][2] = true;
            }
            break;
        case PieceType::J:
            if (height >= 3 && width >= 3) {
                canvas[0][0] = true;
                canvas[1][0] = canvas[1][1] = canvas[1][2] = true;
            }
            break;
        case PieceType::L:
            if (height >= 3 && width >= 3) {
                canvas[0][2] = true;
                canvas[1][0] = canvas[1][1] = canvas[1][2] = true;
            }
            break;
        default:
            // Empty or no piece
            break;
    }

    // Render the piece
    std::vector<Element> pieceRows;
    for (int y = 0; y < height; y++) {
        std::vector<Element> cells;
        for (int x = 0; x < width; x++) {
            const std::string cellStr = canvas[y][x] ? "██" : "  ";
            cells.push_back(text(cellStr) | colorForValue(static_cast<int>(type)));
        }
        pieceRows.push_back(hbox(cells));
    }

    return vbox(pieceRows);
}

Element renderBox(const std::string &title, Element content) {
    return window(text(title) | bold | color(Color::White), vbox({content}));
}

Element renderStats(const int score, const int level, const int linesCleared) {
    Element scoreElement = hbox({
        text("Score: ") | color(Color::White),
        text(std::to_string(score)) | color(Color::Green)
    });

    Element levelElement = hbox({
        text("Level: ") | color(Color::White),
        text(std::to_string(level)) | color(Color::Green)
    });

    Element linesElement = hbox({
        text("Lines: ") | color(Color::White),
        text(std::to_string(linesCleared)) | color(Color::Green)
    });

    return window(text("STATS") | bold | color(Color::White),
                  vbox({scoreElement, levelElement, linesElement}));
}

// Control help for player view
Element renderControls() {
    return window(text("CONTROLS") | bold | color(Color::White),
                  vbox({
                      text("A/D: Move") | color(Color::Yellow),
                      text("Q/E: Rotate") | color(Color::Yellow),
                      text("↓: Down") | color(Color::Yellow),
                      text("Space: Drop") | color(Color::Yellow),
                      text("C: Hold") | color(Color::Yellow),
                      text("B: Bonus") | color(Color::Yellow),
                      text("M: Malus") | color(Color::Yellow),
                      text("Esc: Exit") | color(Color::Yellow)
                  }));
}

void showGameScreen(ClientSession &session) {
    auto screen = ScreenInteractive::TerminalOutput();

    // Track player view (for cycling through opponents)
    bool isSpectator = false;
    std::string errorMessage;
    constexpr bool darkMode = false;

    // Check player status to determine if we're already in a game
    ClientStatus status = session.getOwnStatus();
    if (status != ClientStatus::IN_GAME) {
        errorMessage = "Not in game! Current status: " + std::to_string(static_cast<int>(status));
    }

    // Create a renderer with continuous polling
    const Component empty = Container::Vertical({});
    const auto renderer = Renderer(empty, [&] {
        // Try to get player state first
        try {
            const PlayerState state = session.getPlayerState();
            isSpectator = false;

            // Main game board
            auto mainBoard = renderBoard(state.playerGrid, darkMode, false);

            // Target opponent board - smaller version
            auto targetBoard = renderBoard(state.targetGrid, false, true);

            // Energy bar
            Element energyBar = text("Energy: N/A") | center;
            if (state.playerEnergy != -1) {
                energyBar = renderEnergyBar(state.playerEnergy);
            }
            // Score and level info
            auto statsPanel = renderStats(state.playerScore, state.playerLevel, state.playerLines);

            // Hold and next pieces
            auto holdPieceDisplay = renderBox("HOLD", renderPiece(state.holdTetro, 4, 5));
            auto nextPieceDisplay = renderBox("NEXT", renderPiece(state.nextTetro, 4, 5));

            // Control help
            auto controls = renderControls();

            // Display layout
            Elements content;

            // Add error message if any
            if (!errorMessage.empty()) {
                content.push_back(text(errorMessage) | color(Color::Red));
            }

            // Add main content - centered layout
            content.push_back(
                hbox({
                    // Left - info panel
                    vbox({
                        statsPanel,
                        holdPieceDisplay,
                        nextPieceDisplay,
                        energyBar,
                        controls
                    }),

                    // Center - main board
                    vbox({
                        mainBoard
                    }) | center,

                    // Right - opponent view
                    vbox({
                        text("Opponent: " + state.targetUsername) | bold | center,
                        targetBoard,
                        text("Press S/W to cycle") | center
                    }) | center
                }) | center
            );

            return vbox(content) | border | color(Color::Green);
        } catch (...) {
            // If player state fails, try spectator state
            try {
                const SpectatorState state = session.getSpectatorState();
                isSpectator = true;

                Elements content;

                // Add error message if any
                if (!errorMessage.empty()) {
                    content.push_back(text(errorMessage) | color(Color::Red));
                }

                // Add spectator content
                content.push_back(text("SPECTATING: " + state.playerUsername) | bold | center);
                content.push_back(separator());

                // Pieces display
                auto holdPieceDisplay = renderBox("HOLD", renderPiece(state.holdTetro, 4, 5));
                auto nextPieceDisplay = renderBox("NEXT", renderPiece(state.nextTetro, 4, 5));

                content.push_back(
                    hbox({
                        // Left side - pieces and stats
                        vbox({
                            holdPieceDisplay,
                            nextPieceDisplay,
                            text("Press Esc to exit") | center
                        }),

                        // Center - main board
                        renderBoard(state.playerGrid, darkMode, false)
                    }) | center
                );

                return vbox(content) | border;
            } catch (...) {
                // If both fail, show waiting message
                // Check if we're in game at all
                ClientStatus currentStatus = session.getOwnStatus();
                if (currentStatus != ClientStatus::IN_GAME) {
                    return vbox({
                               text("Not in game - Status: " + std::to_string(static_cast<int>(currentStatus))),
                               text("Press Esc to return to menu")
                           }) | center | border;
                }

                return text("Waiting for game state...") | center | border;
            }
        }
    });

    // Add key event handler
    auto rendererWithKeys = CatchEvent(renderer, [&](Event event) {
        // Only handle keys if not in spectator mode
        if (isSpectator) {
            if (event == Event::Escape) {
                StatusCode result = session.leaveLobby();
                if (result == StatusCode::SUCCESS) {
                    currentScreen = ScreenState::MainMenu;
                    screen.Exit();
                } else {
                    errorMessage = "Failed to leave lobby: " + getStatusCodeString(result);
                }
                return true;
            }
            return false;
        }

        // Game controls for player mode
        if (event == Event::Character('a')) {
            StatusCode result = session.sendKeyStroke(Action::MoveLeft);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('d')) {
            StatusCode result = session.sendKeyStroke(Action::MoveRight);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::ArrowDown) {
            StatusCode result = session.sendKeyStroke(Action::MoveDown);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('e')) {
            StatusCode result = session.sendKeyStroke(Action::RotateRight);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('q')) {
            StatusCode result = session.sendKeyStroke(Action::RotateLeft);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character(' ')) {
            StatusCode result = session.sendKeyStroke(Action::InstantFall);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('c')) {
            StatusCode result = session.sendKeyStroke(Action::UseBag);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('b')) {
            StatusCode result = session.sendKeyStroke(Action::UseBonus);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('m')) {
            StatusCode result = session.sendKeyStroke(Action::UseMalus);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('s')) {
            StatusCode result = session.sendKeyStroke(Action::SeePreviousOpponent);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::Character('w')) {
            StatusCode result = session.sendKeyStroke(Action::SeeNextOpponent);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        // if (event == Event::Character('d')) {
        //     // Toggle dark mode (for testing)
        //     darkMode = !darkMode;
        //     return true;
        // }
        if (event == Event::Escape) {
            // Check current status to determine if we're in lobby or game
            ClientStatus currentStatus = session.getOwnStatus();
            if (currentStatus == ClientStatus::IN_GAME) {
                StatusCode result = session.leaveGame();
                if (result == StatusCode::SUCCESS) {
                    currentScreen = ScreenState::MainMenu;
                    screen.Exit();
                } else {
                    errorMessage = "Failed to leave lobby: " + getStatusCodeString(result);
                }
            } else {
                currentScreen = ScreenState::MainMenu;
                screen.Exit();
            }
            return true;
        }

        return false;
    });

    // Controlled polling
    std::atomic_bool running{true};
    std::mutex mtx;
    std::condition_variable cv;

    // Start the polling thread
    std::thread pollingThread([&screen, &running, &mtx, &cv] {
        std::unique_lock lock(mtx);
        while (running) {
            // Wait for 50ms or until notified
            if (cv.wait_for(lock, std::chrono::milliseconds(50)) == std::cv_status::timeout) {
                screen.PostEvent(Event::Custom);
            }
        }
    });

    // Main event loop
    screen.Loop(rendererWithKeys);

    // After exiting the loop, signal the polling thread to stop.
    running = false;
    cv.notify_all();
    if (pollingThread.joinable()) {
        pollingThread.join();
    }
}
