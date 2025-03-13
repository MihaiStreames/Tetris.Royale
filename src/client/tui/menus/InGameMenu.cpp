#include "InGameMenu.hpp"

using namespace ftxui;

struct str
{
    ftxui::Element scoreTxt = ftxui::text("Score : ");
    ftxui::Element nextTxt = ftxui::text("Next :");
    ftxui::Element powerUpTxt = ftxui::text("Power-Up :");
    ftxui::Element bagTxt = ftxui::text("Bag :");
    ftxui::Element useSpaceTxt = ftxui::text("Use : [SPACE]");
    ftxui::Element targetPlayerTxt = ftxui::text("  Player 2");
    ftxui::Element switchSTxt = ftxui::text("Switch : [s]");
    ftxui::Element spectatorTxt = ftxui::text("Spectator : 0");
};

Decorator
colorForValue(const int v)
{
    switch (v)
    {
    case 1:
        return color(Color::Yellow);
    case 2:
        return color(Color::LightGoldenrod1);
    case 3:
        return color(Color::Green);
    case 4:
        return color(Color::Magenta);
    case 5:
        return color(Color::Red);
    case 6:
        return color(Color::Cyan);
    case 7:
        return color(Color::Blue);
    case 8:
        return color(Color::GrayDark);
    default:
        return color(Color::Default);
    }
}

Element
renderPowerUpBox()
{
    return vbox({}) | border | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 6) |
           color(Color::Green);
}

Element
renderScoreBox(const int score)
{
    str s;
    const Element stringScore = text(std::to_string(score));
    return hbox(
        {s.scoreTxt | color(Color::Green), stringScore | color(Color::Green)});
}

Element
renderProgressBar(int energy)
{

    const int filledCells = energy * MAX_ENERGY / 100;

    std::vector<Element> barElements;
    for (int i = 0; i < MAX_ENERGY; ++i)
    {
        if (i < filledCells)
        {
            barElements.push_back(text("███") | color(Color::Green));
        }
        else
        {
            barElements.push_back(text("  ") | color(Color::Default));
        }
    }

    if (energy > MAX_ENERGY)
        energy = MAX_ENERGY;

    return vbox({vbox(std::move(barElements)) | border,
                 text(std::to_string(energy) + "%") | center |
                     color(Color::Green)}) |
           size(WIDTH, EQUAL, 4);
}

Element
renderSwitchBox()
{
    str s;
    return vbox({s.switchSTxt | color(Color::Green)}) | border |
           color(Color::Green);
}

Element
renderBoard(tetroMat board)
{
    const int height = static_cast<int>(board.size());
    if (height == 0)
    {
        return text("Empty board!") | color(Color::Green);
    }

    const int width = static_cast<int>(board[0].size());
    std::vector<Element> rows;
    rows.reserve(height);

    for (int y = 0; y < height; ++y)
    {
        std::vector<Element> cells;
        cells.reserve(width);

        for (int x = 0; x < width; ++x)
        {
            const int val = board[y][x];
            const std::string cellStr = val != static_cast<int>(PieceType::None) ? "██" : "  ";
            cells.push_back(text(cellStr) | colorForValue(val));
        }

        rows.push_back(hbox(std::move(cells)));
    }

    return vbox(std::move(rows)) | border | center | color(Color::Green);
}

Element
renderTargetPlayerBox(tetroMat targetPlayerBoard)
{
    return renderBoard(targetPlayerBoard);
}

Element
renderPiece(const Tetromino* piece, const int h, const int w)
{
    if (!piece)
    {
        std::vector<Element> emptyRows;
        for (int y = 0; y < h; ++y)
        {
            std::vector<Element> cells;

            for (int x = 0; x < w; ++x)
            {
                cells.emplace_back(text("  "));
            }

            emptyRows.emplace_back(hbox(std::move(cells)));
        }

        return vbox(std::move(emptyRows));
    }

    // Fill a small 2D buffer
    std::vector canvas(h, std::vector(w, 0));
    const int pieceTypeVal = static_cast<int>(piece->getPieceType());
    const auto& shape = piece->getShape();

    for (int y = 0; y < static_cast<int>(shape.size()) && y < h; ++y)
    {
        for (int x = 0; x < static_cast<int>(shape[y].size()) && x < w; ++x)
        {
            canvas[y][x] = shape[y][x] ? pieceTypeVal : static_cast<int>(PieceType::None);
        }
    }

    std::vector<Element> pieceRows;
    for (int row = 0; row < h; ++row)
    {
        std::vector<Element> cells;

        for (int col = 0; col < w; ++col)
        {
            const int val = canvas[row][col];
            const std::string cellStr = val != 0 ? "██" : "  ";
            cells.push_back(text(cellStr) | colorForValue(val));
        }

        pieceRows.push_back(hbox(std::move(cells)));
    }

    return vbox(std::move(pieceRows));
}

Element
renderBox(Element content)
{
    return vbox({std::move(content)}) | border | center | color(Color::Green);
}

Element
renderMainBox(const PlayerState& state)
{
    str s;
    return vbox(
        
        {renderScoreBox(state.playerScore) | center,
                
            // !! state.energy needs to be added but for now MAX_ENERGY is used
            hbox({
                renderProgressBar(MAX_ENERGY), renderBoard(state.playerGrid)
            }) | center,
            
            hbox({
                
                vbox({s.nextTxt | center | color(Color::Green), renderBox(renderPiece(&state.nextTetro, 4, 5)) | center,}),
                vbox({s.bagTxt | center | color(Color::Green), renderBox(renderPiece(&state.holdTetro, 4, 5)) | center, })

            }),
            
            s.powerUpTxt | color(Color::Green),
            renderPowerUpBox(),
            s.useSpaceTxt | color(Color::Green)
        
        }
                
        ) | border | size(WIDTH, EQUAL, 35) | size(HEIGHT, EQUAL, 42) | color(Color::Green);
    
}

Element
inGameMenu(const PlayerState& state)
{
    str s;
    return vbox({hbox({
                     renderMainBox(state),
                     vbox({renderTargetPlayerBox(state.targetGrid),
                           s.targetPlayerTxt | color(Color::Green),
                           renderSwitchBox()}),
                 }) | center,
                 s.spectatorTxt | center | color(Color::Green)}) |
           center;
}

