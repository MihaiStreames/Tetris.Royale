#pragma once

#include <utility>
#include <vector>
#include <string>
#include <ftxui/dom/elements.hpp>
#include "../common/tetrisGame.hpp"

class FtxuiView {
    TetrisGame &tetrisGame;

public:
    explicit FtxuiView(TetrisGame &game) : tetrisGame(game) {}

    [[nodiscard]] ftxui::Element render() const;

private:
    static ftxui::Decorator colorForValue(const int v);

    [[nodiscard]] ftxui::Element renderBoard() const;

    static ftxui::Element renderPiece(const Tetromino *piece, int height, int width);

    [[nodiscard]] static ftxui::Element renderBox(const std::string &title, ftxui::Element content);

    [[nodiscard]] ftxui::Element renderStats() const;
};