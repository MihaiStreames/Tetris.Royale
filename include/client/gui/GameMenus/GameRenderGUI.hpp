#ifndef GAMERENDERGUI_HPP
#define GAMERENDERGUI_HPP

#include <QWidget>
#include <QColor>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QGroupBox>
#include <QProgressBar>
#include <QString>

#include <vector>

#include "Types.hpp"
#include "Common.hpp"
#include "Config.hpp"


QWidget* renderBoard(const tetroMat &board, bool isOpponentBoard, bool isGameOver, QWidget *parent = nullptr);
QWidget* renderEnergyBar(int energy, int maxEnergy = MAX_ENERGY, QWidget *parent = nullptr);
QWidget* renderPiece(PieceType type, int gridHeight, int gridWidth, QWidget *parent = nullptr);
QWidget* renderStats(int score, int level, int linesCleared, QWidget *parent = nullptr);
QWidget* renderControls(Config &config, QWidget *parent = nullptr);
QWidget* renderBox(const QString &title, QWidget *content, QWidget *parent = nullptr);
void placePieceInBoard(PieceType type, int x, int y, tetroMat &board);


#endif // GAMERENDERGUI_HPP
