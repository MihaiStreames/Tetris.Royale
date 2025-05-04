#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <chrono>
#include <thread>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "MenuHandlerTUI.hpp"
#include "ClientSession.hpp"
#include "Common.hpp"

// Forward declaration of the currentScreen variable which is defined in MenuHandler.cpp
extern ScreenState currentScreen;

/**
 * Shows the game screen with the Tetris board, controls, and other UI elements.
 * Handles keyboard input to control the game.
 *
 * @param session The client session used to interact with the game server
 */
void showGameScreen(ClientSession &session, Config &config);

/**
 * Helper function to render a tetris board
 *
 * @param board The tetris board matrix
 * @param darkMode Whether to render in dark mode (reduces visibility)
 * @param isOpponentBoard Whether this is an opponent board (smaller cells)
 * @return An ftxui Element representing the tetris board
 */
ftxui::Element renderBoard(const tetroMat &board, bool darkMode, bool isOpponentBoard, bool isGameOver);

/**
 * Helper function to render a tetromino piece
 *
 * @param type The type of tetromino to render
 * @param height The height of the rendering area
 * @param width The width of the rendering area
 * @return An ftxui Element representing the tetromino
 */
ftxui::Element renderPiece(PieceType type, int height, int width);

/**
 * Helper function to render a box with a title
 *
 * @param title The title of the box
 * @param content The content of the box
 * @return An ftxui Element representing the titled box
 */
ftxui::Element renderBox(const std::string &title, ftxui::Element content);

/**
 * Helper function to render the game statistics
 *
 * @param score The player's score
 * @param level The player's level
 * @param linesCleared The number of lines cleared by the player
 * @return An ftxui Element representing the game statistics
 */
ftxui::Element renderStats(int score, int level, int linesCleared);

/**
 * Helper function to render the energy bar
 *
 * @param energy The current energy level
 * @return An ftxui Element representing the energy bar
 */
ftxui::Element renderEnergyBar(int energy);

/**
 * Helper function to render the game controls
 *
 * @return An ftxui Element representing the game controls
 */
ftxui::Element renderControls(Config &config);

/**
 * Helper function to get the appropriate color for a tetromino block
 *
 * @param value The value representing the tetromino type
 * @return An ftxui Decorator with the appropriate color
 */
ftxui::Decorator colorForValue(int value);