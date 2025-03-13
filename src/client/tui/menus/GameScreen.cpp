#include "GameScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

// Helper to color tetromino blocks
Decorator colorForValue(int value) {
    switch (value) {
        case 1: return color(Color::CyanLight);    // I
        case 2: return color(Color::Yellow);       // O
        case 3: return color(Color::Blue);         // J
        case 4: return color(Color::RedLight);     // Z
        case 5: return color(Color::GreenLight);   // S
        case 6: return color(Color::Magenta);      // T
        case 7: return color(Color::Red);          // L
        case 8: return color(Color::White);        // Ghost/preview
        default: return color(Color::Default);
    }
}

// Render a tetris board
Element renderBoard(const tetroMat& board, bool darkMode = false) {
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

    return vbox(rows) | border;
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
Element renderTetromino(PieceType type) {
    std::string display;

    switch (type) {
        case PieceType::I:
            display = "████";
            break;
        case PieceType::O:
            display = "██\n██";
            break;
        case PieceType::T:
            display = " █ \n███";
            break;
        case PieceType::S:
            display = " ██\n██ ";
            break;
        case PieceType::Z:
            display = "██ \n ██";
            break;
        case PieceType::J:
            display = "█  \n███";
            break;
        case PieceType::L:
            display = "  █\n███";
            break;
        default:
            display = "None";
    }

    int value = static_cast<int>(type);
    return text(display) | colorForValue(value);
}

void showGameScreen(ClientSession& session) {
    auto screen = ScreenInteractive::TerminalOutput();

    // Track player view (for cycling through opponents)
    int currentOpponent = 0;
    bool isSpectator = false;

    // Create a renderer with key event handling
    Component empty = Container::Vertical({});
    auto renderer = Renderer(empty, [&] {
        try {
            // Try to get player state first
            PlayerState state = session.getPlayerState();
            isSpectator = false;

            // Main game board
            auto mainBoard = renderBoard(state.playerGrid);

            // Target opponent board
            auto targetBoard = renderBoard(state.targetGrid);

            // Energy bar
            auto energyBar = renderEnergyBar(MAX_ENERGY);

            // Score and level info
            auto infoPanel = vbox({
                text("Score: " + std::to_string(state.playerScore)),
                text("Level: " + std::to_string(state.playerLevel)),
                separator(),
                text("Hold:"),
                renderTetromino(state.holdTetro) | border,
                separator(),
                text("Next:"),
                renderTetromino(state.nextTetro) | border,
                separator(),
                energyBar
            });

            // Control help
            auto controls = vbox({
                text("Controls:") | bold,
                text("← → : Move"),
                text("↑ : Rotate"),
                text("↓ : Down"),
                text("Space : Drop"),
                text("C : Hold"),
                text("B : Bonus"),
                text("M : Malus"),
                text("S : Switch View"),
                text("Esc : Exit")
            });

            return hbox({
                // Left - info panel
                infoPanel | border | size(WIDTH, EQUAL, 20),

                // Center - main board
                mainBoard | border | size(WIDTH, EQUAL, 22),

                // Right - opponent & controls
                vbox({
                    text("Opponent: " + state.targetUsername) | bold | center,
                    targetBoard,
                    separator(),
                    controls
                }) | border | size(WIDTH, EQUAL, 22)
            }) | border;

        } catch (...) {
            // If player state fails, try spectator state
            try {
                SpectatorState state = session.getSpectatorState();
                isSpectator = true;

                return vbox({
                    text("SPECTATING: " + state.playerUsername) | bold | center,
                    separator(),
                    renderBoard(state.playerGrid) | border | size(WIDTH, EQUAL, 22),
                    hbox({
                        vbox({
                            text("Hold:"),
                            renderTetromino(state.holdTetro) | border
                        }),
                        vbox({
                            text("Next:"),
                            renderTetromino(state.nextTetro) | border
                        })
                    }),
                    text("Press Esc to exit") | center
                }) | border;

            } catch (...) {
                // If both fail, show waiting message
                return text("Waiting for game state...") | center | border;
            }
        }
    });

    // Handle keyboard input
    auto rendererWithKeys = CatchEvent(renderer, [&](Event event) {
        // Only handle keys if not in spectator mode
        if (isSpectator) {
            if (event == Event::Escape) {
                session.leaveLobby();
                currentScreen = ScreenState::MainMenu;
                screen.Exit();
                return true;
            }
            return false;
        }

        // Game controls
        if (event == Event::ArrowLeft) {
            session.sendKeyStroke(Action::MoveLeft);
            return true;
        }
        if (event == Event::ArrowRight) {
            session.sendKeyStroke(Action::MoveRight);
            return true;
        }
        if (event == Event::ArrowDown) {
            session.sendKeyStroke(Action::MoveDown);
            return true;
        }
        if (event == Event::Character('e')) {
            session.sendKeyStroke(Action::RotateRight);
            return true;
        }
        if (event == Event::Character('q')) {
            session.sendKeyStroke(Action::RotateLeft);
            return true;
        }
        if (event == Event::Character(' ')) {
            session.sendKeyStroke(Action::InstantFall);
            return true;
        }
        if (event == Event::Character('c')) {
            session.sendKeyStroke(Action::UseBag);
            return true;
        }
        if (event == Event::Character('b')) {
            session.sendKeyStroke(Action::UseBonus);
            return true;
        }
        if (event == Event::Character('m')) {
            session.sendKeyStroke(Action::UseMalus);
            return true;
        }
        if (event == Event::Character('s')) {
            session.sendKeyStroke(Action::SeePreviousOpponent);
            return true;
        }
        if (event == Event::Character('w')) {
            session.sendKeyStroke(Action::SeeNextOpponent);
            return true;
        }
        if (event == Event::Escape) {
            session.leaveLobby();
            currentScreen = ScreenState::MainMenu;
            screen.Exit();
            return true;
        }

        return false;
    });

    // Main loop
    screen.Loop(rendererWithKeys);
}