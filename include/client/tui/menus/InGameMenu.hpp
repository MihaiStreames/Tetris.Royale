#pragma once

#include "ViewTestData.hpp"
#include <ftxui/dom/elements.hpp>
#include <string>
#include <utility>
#include <vector>

#include "GameMatrix.hpp"
#include "GameState.hpp"
#include "Common.hpp"
#include "Types.hpp"
#include "Tetromino.hpp"

struct str;

// Render routine
ftxui::Element inGameMenu(const PlayerState& state);

static ftxui::Decorator colorForValue(int v);

ftxui::Element renderMainBox(const PlayerState& state);

static ftxui::Element renderPowerUpBox();

ftxui::Element renderTargetPlayerBox(tetroMat targetGrid);

ftxui::Element renderScoreBox(int score);

static ftxui::Element renderProgressBar(int energy);

ftxui::Element renderSwitchBox();

ftxui::Element renderBoard(tetroMat board);

static ftxui::Element renderPiece(const Tetromino* piece, int h, int w);

static ftxui::Element renderBox(ftxui::Element content);