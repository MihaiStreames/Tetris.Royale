#ifndef GAMESCREENGUI_HPP
#define GAMESCREENGUI_HPP

#include <QWidget>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QSpacerItem>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

#include <iostream>

#include "Types.hpp"
#include "Common.hpp"
#include "Config.hpp"
#include "ClientSession.hpp"
#include "GameRenderGUI.hpp"
#include "MainMenuGUI.hpp"           


class GameScreen : public QWidget {
    
    Q_OBJECT

public:
    explicit GameScreen(Config &config, ClientSession &session,const QString &role, QWidget *parent = nullptr);
    ~GameScreen() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onUpdateTimer();
    void onEscapePressed();

private:
    void setupUi();
    void setupPlayerUI(const PlayerState &ps);
    void setupSpectatorUI(const SpectatorState &ss);
    void clearLayout(QLayout *layout);
    void replaceWidgetInLayout(QBoxLayout *layout, QWidget *&oldW, QWidget *newW);

    Config        &configRef;
    ClientSession &session;
    tetroMat playerBoard;
    tetroMat opponentBoard;
    bool     isSpectator = false;

    QWidget *mainBoardWidget    = nullptr;
    QWidget *oppBoardWidget     = nullptr;
    QWidget *statsWidget        = nullptr;
    QWidget *holdWidget         = nullptr;
    QWidget *nextWidget         = nullptr;
    QWidget *energyWidget       = nullptr;
    QWidget *controlsWidget     = nullptr;
    QWidget *opponentContainer  = nullptr;
    QLabel  *opponentNameLabel  = nullptr;

    QTimer  *updateTimer        = nullptr;

    QVBoxLayout *leftColLayout   = nullptr;
    QVBoxLayout *middleColLayout = nullptr;
    QVBoxLayout *rightColLayout  = nullptr;

    QString userRole;

};


#endif // GAMESCREEN_HPP
