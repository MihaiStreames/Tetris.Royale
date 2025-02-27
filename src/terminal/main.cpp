#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "tetris-common/tetrisGame.hpp"
#include "tetris-common/classicGame.hpp"
#include "tetris-common/royalGame.hpp"
#include "tetris-common/gameEngine.hpp"
#include "tetris-common/classicEngine.hpp"
#include "tetris-common/royalEngine.hpp"
#include "view/tui.hpp"
#include "view/inputHandler.hpp"

using namespace ftxui;

int main() {
    RoyalGame royal_game{10, 22};
    TetrisGame& game = royal_game;
    RoyalEngine engine; // je du l ajouter car j ai enlever les static
    const FtxuiView view(game);
    inputHandler ih;

    auto screen = ScreenInteractive::Fullscreen();

    std::mutex mtx;
    std::atomic running(true);

    // Renderer component
    const auto renderer = Renderer([&] {
        std::lock_guard lock(mtx);
        return view.render();
    });

    // Event handler component
    const auto event_component = CatchEvent(renderer, [&](const Event& event) -> bool {
        std::lock_guard lock(mtx);

        if (event == Event::Escape) {
            // Exit the game when Escape is pressed
            running = false;
            screen.Exit();
            return true;
        }

        ih.setEvent(event);
        return false;
    });

    // Game loop in separate thread
    std::thread game_thread([&] {
        constexpr double REFRESH_RATE = 60.0;
        using clock = std::chrono::steady_clock;
        auto next_frame = clock::now();

        while (running && !game.isGameOver()) {
            {
                std::lock_guard lock(mtx);
                if(game.getReverseControls()){
                    ih.invertKeys();
                    game.inverted_command(false); // --
                }
                const Action action = ih.handleInputs();
                engine.handlingRoutine(game, action);
            }

            screen.Post(Event::Custom);

            next_frame += std::chrono::milliseconds(static_cast<int>(1000.0 / REFRESH_RATE));
            std::this_thread::sleep_until(next_frame);
        }

        running = false;
        screen.Exit();
    });

    screen.Loop(event_component);

    if (game_thread.joinable()) game_thread.join();
    return 0;
}