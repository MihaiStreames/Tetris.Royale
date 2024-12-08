#include <ftxui/component/component.hpp>       // for Renderer, etc.
#include <ftxui/component/screen_interactive.hpp> // for ScreenInteractive
#include "tetris-common/tetrisGame.hpp"
#include "view/tui.hpp"

int main() {
    TetrisGame game(10, 22);
    const FtxuiView view(game);

    auto screen = ScreenInteractive::TerminalOutput();

    const auto renderer = Renderer([&] {
        return view.render();
    });

    screen.Loop(renderer);

    return 0;
}