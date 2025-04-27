#ifndef GAMESCREEN_HPP
#define GAMESCREEN_HPP

#include <QWidget>
#include <QMainWindow>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#include "Types.hpp"              // tetroMat, PieceType, MAX_ENERGY
#include "Common.hpp"             // Action
#include "Config.hpp"
#include "ClientSession.hpp"
#include "GameRender.hpp"   // renderBoard, renderStats, ...
#include "../MainMenus/mainMenu.hpp"           

class GameScreen : public QWidget
{
    Q_OBJECT

public:
    /// Constructeur principal : prend la Config et la ClientSession en référence
    explicit GameScreen(Config &config, ClientSession &session, QWidget *parent = nullptr);
    ~GameScreen() override;

protected:
    /// Dessine le fond (image Tetris)
    void paintEvent(QPaintEvent *event) override;

private slots:
    /// Slot déclenché périodiquement pour rafraîchir l’affichage
    void onUpdateTimer();

    /// Gère l’appui sur Échap en cours de partie et met à jour le meilleur score en mode solo (ENDLESS)
    void onEscapePressed();

private:
    // ----------------------------------------
    // Méthodes d’initialisation de l’UI
    // ----------------------------------------
    void setupUi();
    void setupPlayerUI(const PlayerState &ps);
    void setupSpectatorUI(const SpectatorState &ss);
    void clearLayout(QLayout *layout);

    // ----------------------------------------
    // Helpers pour remplacer dynamiquement un widget
    // ----------------------------------------
    void replaceWidgetInLayout(QBoxLayout *layout, QWidget *&oldW, QWidget *newW);

    // ----------------------------------------
    // Références externes
    // ----------------------------------------
    Config        &configRef;
    ClientSession &session;

    // ----------------------------------------
    // Données du jeu
    // ----------------------------------------
    tetroMat playerBoard;
    tetroMat opponentBoard;
    bool     isSpectator = false;

    // ----------------------------------------
    // Widgets principaux
    // ----------------------------------------
    QWidget *mainBoardWidget    = nullptr;
    QWidget *oppBoardWidget     = nullptr;
    QWidget *statsWidget        = nullptr;
    QWidget *holdWidget         = nullptr;
    QWidget *nextWidget         = nullptr;
    QWidget *energyWidget       = nullptr;
    QWidget *controlsWidget     = nullptr;

    // ----------------------------------------
    // Conteneur et label pour l’adversaire
    // ----------------------------------------
    QWidget *opponentContainer  = nullptr;
    QLabel  *opponentNameLabel  = nullptr;

    // ----------------------------------------
    // Timer de mise à jour
    // ----------------------------------------
    QTimer  *updateTimer        = nullptr;

    // ----------------------------------------
    // Layouts pour replacement
    // ----------------------------------------
    QVBoxLayout *leftColLayout   = nullptr;
    QVBoxLayout *middleColLayout = nullptr;
    QVBoxLayout *rightColLayout  = nullptr;
};

#endif // GAMESCREEN_HPP
