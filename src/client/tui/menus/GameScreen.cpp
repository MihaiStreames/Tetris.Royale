#include "GameScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

Decorator colorForValue(int value) {
    switch (value) {
        case 1: return color(Color::Yellow);          // I
        case 2: return color(Color::LightGoldenrod1); // O
        case 3: return color(Color::Green);           // J
        case 4: return color(Color::Magenta);         // Z
        case 5: return color(Color::Red);             // S
        case 6: return color(Color::Cyan);            // T
        case 7: return color(Color::Blue);            // L
        case 8: return color(Color::White);           // Ghost/preview
        default: return color(Color::Default);
    }
}

// Render a tetris board
Element renderBoard(const tetroMat &board, bool darkMode = false) {
    if (board.empty()) {
        return text("No board data") | center;
    }

    const int height = static_cast<int>(board.size());
    const int width = static_cast<int>(board[0].size());

    Elements rows;
    for (int y = 0; y < height; y++) {
        Elements cells;
        for (int x = 0; x < width; x++) {
            std::string cellStr;
            if (darkMode && board[y][x] != 0) {
                cellStr = "██"; // In dark mode, only show filled cells
            } else if (!darkMode) {
                cellStr = board[y][x] == 0 ? "  " : "██";
            } else {
                cellStr = "  "; // Empty in dark mode
            }

            cells.push_back(text(cellStr) | colorForValue(board[y][x]));
        }
        rows.push_back(hbox(cells));
    }

    return window(text("BOARD") | bold | color(Color::White), vbox(rows));
}

// Render energy bar
Element renderEnergyBar(int energy) {
    const int maxEnergy = MAX_ENERGY;
    const int filledCells = (energy * 10) / maxEnergy;

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
Element renderPiece(PieceType type, int height = 4, int width = 5) {
    // Initialize an empty canvas grid
    std::vector<std::vector<bool>> canvas(height, std::vector<bool>(width, false));

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

Element renderStats(int score, int level, int linesCleared) {
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
            text("←/→: Move") | color(Color::Yellow),
            text("Q/E: Rotate") | color(Color::Yellow),
            text("↓: Down") | color(Color::Yellow),
            text("Space: Drop") | color(Color::Yellow),
            text("C: Hold") | color(Color::Yellow),
            text("B: Bonus") | color(Color::Yellow),
            text("M: Malus") | color(Color::Yellow),
            text("S/W: Switch View") | color(Color::Yellow),
            text("Esc: Exit") | color(Color::Yellow)
        }));
}

void showGameScreen(ClientSession &session) {
    auto screen = ScreenInteractive::TerminalOutput();

    // Track player view (for cycling through opponents)
    bool isSpectator = false;
    std::string errorMessage;
    bool darkMode = false;

    // Check player status to determine if we're already in a game
    ClientStatus status = session.getOwnStatus();
    if (status != ClientStatus::IN_GAME) {
        errorMessage = "Not in game! Current status: " + std::to_string(static_cast<int>(status));
    }

    // For controlling frame rate
    int frameCounter = 0;
    constexpr int FRAME_RATE = 60;

    // Create a renderer with key event handling
    Component empty = Container::Vertical({});
    auto renderer = Renderer(empty, [&] {
        // Increment frame counter for periodic updates
        frameCounter++;
        if (frameCounter >= FRAME_RATE) {
            frameCounter = 0;

            // Periodically check if we're still in game
            // ClientStatus currentStatus = session.getOwnStatus();
            // if (currentStatus != ClientStatus::IN_GAME) {
            //     // If we're no longer in game and not explicitly leaving
            //     currentScreen = ScreenState::MainMenu;
            //     screen.Exit();
            //     return text("Game ended") | center;
            // }
        }

        try {
            // Try to get player state first
            PlayerState state = session.getPlayerState();
            isSpectator = false;

            // Main game board
            auto mainBoard = renderBoard(state.playerGrid, darkMode);

            // Target opponent board
            auto targetBoard = renderBoard(state.targetGrid);

            // Energy bar
            auto energyBar = renderEnergyBar(MAX_ENERGY);

            // Score and level info
            auto statsPanel = renderStats(state.playerScore, state.playerLevel, 0);

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

            // Add main content
            content.push_back(
                hbox({
                    // Left - info panel
                    vbox({
                        statsPanel,
                        holdPieceDisplay,
                        nextPieceDisplay,
                        energyBar,
                        controls
                    }) | size(WIDTH, EQUAL, 20) | color(Color::Green),

                    // Center - main board
                    mainBoard | size(WIDTH, EQUAL, 22) | color(Color::Green),

                    // Right - opponent view
                    vbox({
                        text("Opponent: " + state.targetUsername) | bold | center,
                        targetBoard | size(WIDTH, EQUAL, 20),
                        text("Press S/W to cycle opponents") | center
                    }) | size(WIDTH, EQUAL, 22) | color(Color::Green)
                })
            );

            return vbox(content) | border | color(Color::Green);
        } catch (...) {
            // If player state fails, try spectator state
            try {
                SpectatorState state = session.getSpectatorState();
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
                        }) | size(WIDTH, EQUAL, 20),

                        // Center - main board
                        renderBoard(state.playerGrid, darkMode) | size(WIDTH, EQUAL, 22)
                    })
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

    // Handle keyboard input
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
        if (event == Event::ArrowLeft) {
            StatusCode result = session.sendKeyStroke(Action::MoveLeft);
            if (result != StatusCode::SUCCESS) {
                errorMessage = "Command failed: " + getStatusCodeString(result);
            }
            return true;
        }
        if (event == Event::ArrowRight) {
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
        if (event == Event::ArrowUp) {
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
                StatusCode result = session.leaveLobby();
                if (result == StatusCode::SUCCESS) {
                    currentScreen = ScreenState::MainMenu;
                    screen.Exit();
                } else {
                    errorMessage = "Failed to leave lobby: " + getStatusCodeString(result);
                }
            } else {
                // If we're in game, just go back to main menu
                currentScreen = ScreenState::MainMenu;
                screen.Exit();
            }
            return true;
        }

        return false;
    });

    // Main loop with automatic refresh
    screen.Loop(rendererWithKeys);
}