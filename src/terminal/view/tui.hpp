#pragma once

#include <utility>
#include <vector>
#include <string>
#include <ftxui/dom/elements.hpp>
#include "tetris-common/tetrisGame.hpp"

using namespace ftxui;

class FtxuiView {
    TetrisGame& tetrisGame;

public:
    explicit FtxuiView(TetrisGame& game) : tetrisGame(game) {}

    [[nodiscard]] Element render() const {
        return hbox({
            renderBoard(),
            separator(),
            vbox({
                renderBox("HOLD", renderPiece(tetrisGame.getBag().peekPiece())),
                separator(),
                renderBox("NEXT", renderPiece(&tetrisGame.getFactory().whatIsNextPiece())),
                separator(),
                renderScore()
            }),
        }) | size(HEIGHT, EQUAL, 22);
    }

private:
    static Decorator colorForValue(const int v) {
        switch (v) {
            case 1: return  color(Color::Yellow);
            case 2: return  color(Color::LightGoldenrod1);
            case 3: return  color(Color::Green);
            case 4: return  color(Color::Magenta);
            case 5: return  color(Color::Red);
            case 6: return  color(Color::Cyan);
            case 7: return  color(Color::Blue);
            default: return color(Color::White);
        }
    }

    [[nodiscard]] Element renderBoard() const {
        const auto& gm = tetrisGame.getGameMatrix();
        const auto board = gm.getBoardWithCurrentPiece();

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

        return window(text("BOARD") | bold | color(Color::White), vbox(std::move(rows)));
    }

    static Element renderPiece(const Tetromino* piece) {
        constexpr int size = 4;

        if (!piece) {
            return text("  ");
        }

        std::vector canvas(size, std::vector(size, 0));

        const int pieceTypeVal = piece->getPieceType();

        const auto& shape = piece->getShape();
        for (int y = 0; y < static_cast<int>(shape.size()) && y < size; ++y) {
            for (int x = 0; x < static_cast<int>(shape[y].size()) && x < size; ++x) {
                canvas[y][x] = shape[y][x];
            }
        }

        std::vector<Element> pieceRows;
        for (int y = 0; y < size; ++y) {
            std::vector<Element> cells;

            for (int x = 0; x < size; ++x) {
                const int val = canvas[y][x] ? pieceTypeVal : 0;
                const std::string cellStr = canvas[y][x] ? "██" : "  ";
                cells.push_back(text(cellStr) | colorForValue(val));
            }

            pieceRows.push_back(hbox(std::move(cells)) | center);
        }

        return vbox(std::move(pieceRows)) | center;
    }

    [[nodiscard]] static Element renderBox(const std::string& title, Element content) {
        return window(text(title) | bold | color(Color::White), vbox({ std::move(content) }));
    }

    [[nodiscard]] Element renderScore() const {
        const std::string scoreText = ">> " + std::to_string(tetrisGame.getScore());
        Element scoreElement = text(scoreText) | bold | color(Color::White) | center;

        return window(text("SCORE") | bold | color(Color::White), vbox({ scoreElement }));
    }
};