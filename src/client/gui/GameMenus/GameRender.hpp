#ifndef GAMERENDER_HPP
#define GAMERENDER_HPP

#include <QWidget>
#include <QColor>
#include <QString>
#include <vector>

// Importation des types et constantes partagés —
// ne plus redéclarer PieceType, Action, Config, MAX_ENERGY, tetroMat...
#include "Types.hpp"            // typedef tetroMat, enum class PieceType, constexpr MAX_ENERGY
#include "Common.hpp"           // enum class Action (et éventuels autres)
#include "Config.hpp"  // class Config

// Fonctions de dessin du plateau
QWidget* renderBoard(const tetroMat &board,
                     bool isOpponentBoard,
                     bool isGameOver,
                     QWidget *parent = nullptr);

// Barre d'énergie
QWidget* renderEnergyBar(int energy,
                         int maxEnergy = MAX_ENERGY,
                         QWidget *parent = nullptr);

// Placement d'une pièce dans la matrice
void placePieceInBoard(PieceType type,
                       int x,
                       int y,
                       tetroMat &board);

// Affichage d'une pièce (I, O, T, etc.)
QWidget* renderPiece(PieceType type,
                     int gridHeight,
                     int gridWidth,
                     QWidget *parent = nullptr);

// Stats (score, niveau, lignes)
QWidget* renderStats(int score,
                     int level,
                     int linesCleared,
                     QWidget *parent = nullptr);

// Affichage des contrôles/commandes
QWidget* renderControls(Config &config,
                        QWidget *parent = nullptr);

// Encapsulation dans une boîte stylée
QWidget* renderBox(const QString &title,
                   QWidget *content,
                   QWidget *parent = nullptr);

#endif // GAMERENDER_HPP
