#pragma once

#include <vector>
#include <string>
#include <ftxui/dom/elements.hpp>
#include "tetris-common/tetrisGame.hpp"

using namespace ftxui;

class FtxuiView {
    TetrisGame& tetrisGame;

public:
    explicit FtxuiView(TetrisGame& game) : tetrisGame(game) {}

    Element render() const {
        // Board | spacing | Bag box | Next piece box | Score box
        // Arranged in a horizontal manner (board on left, info on right)
        return hbox({
            renderBoard(),
            separator(),
            vbox({
                renderBag(),
                separator(),
                renderNextPiece(),
                separator(),
                renderScore()
            })
        });
    }

private:
    static Decorator colorForValue(const int v) {
        switch (v) {
            case 1: return color(Color::Yellow);
            case 2: return color(Color::LightGoldenrod1);
            case 3: return color(Color::Green);
            case 4: return color(Color::Magenta);
            case 5: return color(Color::Red);
            case 6: return color(Color::Cyan);
            case 7: return color(Color::Blue);
            default: return color(Color::White);
        }
    }

    Element renderBoard() const {
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

        // Add a border and a title
        return window(text("BOARD"), vbox(std::move(rows)));
    }

    static Element renderSinglePiece(const std::string& title, const Tetromino* piece) {
        // A 4x4 canvas
        constexpr int size = 4;
        std::vector canvas(size, std::vector(size,0));

        int pieceTypeVal = 0;
        if (piece) {
            const auto& shape = piece->getShape();
            pieceTypeVal = static_cast<int>(piece->getPieceType());

            for (int y = 0; y < static_cast<int>(shape.size()) && y < size; ++y) {
                for (int x = 0; x < static_cast<int>(shape[y].size()) && x < size; ++x) {
                    canvas[y][x] = shape[y][x];
                }
            }
        }

        std::vector<Element> rows;
        rows.push_back(text(" " + title + " "));

        for (int y = 0; y < size; ++y) {
            std::vector<Element> cells;

            for (int x = 0; x < size; ++x) {
                const int val = canvas[y][x] ? pieceTypeVal : 0;
                const std::string cellStr = canvas[y][x] != 0 ? "██" : "  ";
                cells.push_back(text(cellStr) | colorForValue(val));
            }

            rows.push_back(hbox(std::move(cells)));
        }

        return window(text(" "), vbox(std::move(rows)));
    }

    Element renderBag() const {
        const Tetromino heldPiece = tetrisGame.getBag().retrievePiece();
        return renderSinglePiece("HOLD", &heldPiece);
    }

    Element renderNextPiece() const {
        const Tetromino nextPiece = tetrisGame.getFactory().whatIsNextPiece();
        return renderSinglePiece("NEXT", &nextPiece);
    }

    Element renderScore() const {
        std::vector<Element> rows;
        rows.push_back(text("== SCORE =="));
        rows.push_back(text(">> " + std::to_string(tetrisGame.getScore())));

        // Put in a window or just a box
        return window(text("SCORE"), vbox(std::move(rows)));
    }
};