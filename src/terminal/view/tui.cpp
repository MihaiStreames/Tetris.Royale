#include "tui.hpp"

using namespace ftxui;

[[nodiscard]] Element FtxuiView::render() const {
    return vbox({
               hbox({
                   renderStats(),

                   renderBoard() | center,

                   vbox({
                       renderBox("HOLD", renderPiece(tetrisGame.getBag().peekPiece(), 4, 5)) | center,
                       renderBox("NEXT", renderPiece(&tetrisGame.getFactory().whatIsNextPiece(), 4, 5)) | center,
                   })

               }) | center,
           }) | center;
}

Decorator FtxuiView::colorForValue(const int v) {
    switch (v) {
        case 1: return  color(Color::Yellow);
        case 2: return  color(Color::LightGoldenrod1);
        case 3: return  color(Color::Green);
        case 4: return  color(Color::Magenta);
        case 5: return  color(Color::Red);
        case 6: return  color(Color::Cyan);
        case 7: return  color(Color::Blue);
        case 8: return  color(Color::GrayDark);
        default: return color(Color::Default);
    }
}

[[nodiscard]] Element FtxuiView::renderBoard() const {
    const auto &gm = tetrisGame.getGameMatrix();
    const auto board = tetrisGame.getDarkMode() ? tetroMat(gm.getHeight(), std::vector<int>(gm.getWidth(), 0)) : gm.getBoardWithCurrentPiece();


    const int width = gm.getWidth();
    const int height = gm.getHeight();

    std::vector<Element> rows;
    rows.reserve(height);

    for (int y = 0; y < height; ++y) {
        std::vector<Element> cells;
        cells.reserve(width);

        for (int x = 0; x < width; ++x) {
            const int val = board[y][x];
            const std::string cellStr = val != 0 ? "██" : "  ";
            cells.push_back(text(cellStr) | colorForValue(val));
        }

        rows.push_back(hbox(std::move(cells)));
    }

    return window(text("BOARD") | bold | color(Color::Default),
        vbox(std::move(rows)));
}

Element FtxuiView::renderPiece(const Tetromino *piece, int height, int width) {
    if (!piece) {
        // Render an empty space with specified size
        std::vector<Element> emptyRows;
        for (int y = 0; y < height; ++y) {
            std::vector<Element> cells;

            for (int x = 0; x < width; ++x) {
                cells.emplace_back(text("  "));
            }

            emptyRows.emplace_back(hbox(std::move(cells)));
        }

        return vbox(std::move(emptyRows));
    }

    // Initialize a canvas grid
    std::vector canvas(height, std::vector(width, 0));

    const int pieceTypeVal = piece->getPieceType();
    const auto &shape = piece->getShape();
    for (int y = 0; y < static_cast<int>(shape.size()) && y < height; ++y) {
        for (int x = 0; x < static_cast<int>(shape[y].size()) && x < width; ++x) {
            canvas[y][x] = shape[y][x];
        }
    }

    std::vector<Element> pieceRows;
    for (int y = 0; y < height; ++y) {
        std::vector<Element> cells;

        for (int x = 0; x < width; ++x) {
            const int val = canvas[y][x] ? pieceTypeVal : 0;
            const std::string cellStr = canvas[y][x] ? "██" : "  ";
            cells.push_back(text(cellStr) | colorForValue(val));
        }

        pieceRows.emplace_back(hbox(std::move(cells)));
    }

    return vbox(std::move(pieceRows));
}

[[nodiscard]] Element FtxuiView::renderBox(const std::string &title, Element content) {
    return window(text(title) | bold | color(Color::Default),
        vbox({std::move(content)}))
    | center;
}

[[nodiscard]] Element FtxuiView::renderStats() const {
    Element scoreElement = hbox({
        text("Score: ") | color(Color::Default),
        text(std::to_string(tetrisGame.getScore())) | color(Color::Green)
    });

    Element levelElement = hbox({
        text("Level: ") | color(Color::Default),
        text(std::to_string(tetrisGame.getLevel())) | color(Color::Green)
    });

    Element linesElement = hbox({
        text("Lines: ") | color(Color::Default),
        text(std::to_string(tetrisGame.getLinesCleared())) | color(Color::Green)
    });

    return window(text("STATS") | bold | color(Color::Default),
        vbox({scoreElement, levelElement, linesElement}))
    | size(HEIGHT, EQUAL, 3) | size(WIDTH, EQUAL, 16);
}