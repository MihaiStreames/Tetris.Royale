
#include "GameScreenGUI.hpp"

GameScreen::GameScreen(Config &config, ClientSession &session,const QString &role, QWidget *parent) : QWidget(parent),
    configRef(config), session(session),userRole(role), playerBoard(20, std::vector<int>(10, 0)), opponentBoard(20, std::vector<int>(10, 0)) {
    // Initial game boards for demo
    placePieceInBoard(PieceType::J, 0, 0, playerBoard);
    placePieceInBoard(PieceType::L, 5, 3, opponentBoard);

    setupUi();

    // Automatic update timer
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &GameScreen::onUpdateTimer);
    updateTimer->start(50);  // 50 ms refresh
}

GameScreen::~GameScreen() {
    if (updateTimer && updateTimer->isActive()) {
        updateTimer->stop();
    }
}

void GameScreen::setupUi()
{
    // this widget
    this->setObjectName("mainWindow");
    auto *mainLayout = new QHBoxLayout(this);

    // Colonnes
    leftColLayout   = new QVBoxLayout;
    middleColLayout = new QVBoxLayout;
    rightColLayout  = new QVBoxLayout;

    // Left column: opponent section
    mainLayout->addLayout(leftColLayout,   1);
    // Middle column: player board
    mainLayout->addLayout(middleColLayout, 2);
    // Right column: stats & controls
    mainLayout->addLayout(rightColLayout,  1);

    // Prepare dynamic widgets
    oppBoardWidget  = renderBoard(opponentBoard, true,  false, this);
    mainBoardWidget = renderBoard(playerBoard,   false, false, this);

    statsWidget    = renderBox("Stats",       renderStats(0,  0,   0, this), this);
    holdWidget     = renderBox("Hold",        renderPiece(PieceType::I, 4, 4, this), this);
    nextWidget     = renderBox("Next",        renderPiece(PieceType::J, 4, 4, this), this);
    energyWidget   = renderBox("Energy Bar",  renderEnergyBar(0, MAX_ENERGY, this), this);
    controlsWidget = renderBox("CONTROLS",    renderControls(configRef, this), this);

    // Opponent container: board + name label
    opponentContainer = new QWidget(this);
    auto *opLayout = new QVBoxLayout(opponentContainer);
    opLayout->setContentsMargins(0, 0, 0, 0);

    opponentNameLabel = new QLabel("Waiting for opponent...", opponentContainer);
    opponentNameLabel->setStyleSheet(R"(
        color: #00FFE0;
        background-color: rgba(30, 30, 30, 180);
        border: 2px dashed rgb(0, 225, 255);
        border-radius: 8px;
        padding: 0 5px;
        font-size: 16px;
    )");
    opponentNameLabel->setFixedHeight(40);
    opponentNameLabel->setMaximumWidth(500);
    opponentNameLabel->setAlignment(Qt::AlignCenter);

    opLayout->addWidget(oppBoardWidget);
    opLayout->addWidget(opponentNameLabel);

    // Assemble in left, middle, and right
    leftColLayout->addWidget(opponentContainer);
    middleColLayout->addWidget(mainBoardWidget);

    rightColLayout->addWidget(statsWidget);
    rightColLayout->addWidget(holdWidget);
    rightColLayout->addWidget(nextWidget);
    rightColLayout->addWidget(energyWidget);
    rightColLayout->addWidget(controlsWidget);
    rightColLayout->addStretch();

    // Column margins
    leftColLayout->setContentsMargins(170, 0, 90, 90);
    middleColLayout->setContentsMargins(120, 15,   0, 15);
    rightColLayout->setContentsMargins(0, 10, 290, 0);

    // Keyboard shortcuts
    auto makeShortcut = [&](const QKeySequence &seq, Action act) {
        auto *sc = new QShortcut(seq, this);
        connect(sc, &QShortcut::activated, [this, act]() {
            (void) session.sendKeyStroke(act);
        });
    };

    makeShortcut(QKeySequence(configRef.get("MoveLeft").c_str()),    Action::MoveLeft);
    makeShortcut(QKeySequence(configRef.get("MoveRight").c_str()),   Action::MoveRight);
    makeShortcut(QKeySequence(configRef.get("MoveDown").c_str()),    Action::MoveDown);
    makeShortcut(QKeySequence(configRef.get("RotateLeft").c_str()),  Action::RotateLeft);
    makeShortcut(QKeySequence(configRef.get("RotateRight").c_str()), Action::RotateRight);
    makeShortcut(QKeySequence(configRef.get("InstantFall").c_str()), Action::InstantFall);
    makeShortcut(QKeySequence(configRef.get("UseBag").c_str()),      Action::UseBag);
    makeShortcut(QKeySequence(configRef.get("UseBonus").c_str()),    Action::UseBonus);
    makeShortcut(QKeySequence(configRef.get("UseMalus").c_str()),    Action::UseMalus);
    makeShortcut(QKeySequence(configRef.get("SeePreviousOpponent").c_str()), Action::SeePreviousOpponent);
    makeShortcut(QKeySequence(configRef.get("SeeNextOpponent").c_str()), Action::SeeNextOpponent);
    makeShortcut(QKeySequence(Qt::Key_Left), Action::MoveLeft);
    makeShortcut(QKeySequence(Qt::Key_Right), Action::MoveRight);
    makeShortcut(QKeySequence(Qt::Key_Down), Action::MoveDown);
    makeShortcut(QKeySequence(Qt::Key_Space), Action::InstantFall);


    // Escape to close window
    auto *esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(esc, &QShortcut::activated, this, &GameScreen::onEscapePressed);

    this->setLayout(mainLayout);
}

void GameScreen::replaceWidgetInLayout(QBoxLayout *layout, QWidget *&oldW, QWidget *newW)
{
    int idx = layout->indexOf(oldW);
    if (idx < 0) return;
    layout->removeWidget(oldW);
    oldW->deleteLater();
    layout->insertWidget(idx, newW);
    oldW = newW;
}

void GameScreen::onUpdateTimer()
{
    if (session.getOwnStatus() != ClientStatus::IN_GAME)
        return;

    if (userRole == "PLAYER") {
        auto ps = session.getPlayerState();
        isSpectator = false;
        setupPlayerUI(ps);
    } else if (userRole == "SPECTATOR") {
        auto ss = session.getSpectatorState();
        isSpectator = true;
        setupSpectatorUI(ss);
    } else {
        qWarning() << "Unknown role: " << userRole;
    }
}


void GameScreen::setupPlayerUI(const PlayerState &ps)
{
    // Update main board
    auto *nbrd = renderBoard(ps.playerGrid, false, ps.isGameOver, this);
    replaceWidgetInLayout(middleColLayout, mainBoardWidget, nbrd);

    // Update opponent view & name
    opponentNameLabel->setText(QString("Spectating : %1").arg(QString::fromStdString(ps.targetUsername)));
    auto *obrd = renderBoard(ps.targetGrid, true, false, opponentContainer);
    opponentContainer->layout()->replaceWidget(oppBoardWidget, obrd);
    oppBoardWidget->deleteLater();
    oppBoardWidget = obrd;

    // Update right-side panels
    auto *nstats  = renderBox("Stats",      renderStats(ps.playerScore, ps.playerLevel, ps.playerLines, this), this);
    auto *nhold   = renderBox("Hold",       renderPiece(ps.holdTetro, 4,4, this),                         this);
    auto *nnext   = renderBox("Next",       renderPiece(ps.nextTetro, 4,4, this),                         this);
    auto *nenergy = renderBox("Energy Bar", renderEnergyBar(ps.playerEnergy, MAX_ENERGY, this),               this);

    replaceWidgetInLayout(rightColLayout, statsWidget,   nstats);
    replaceWidgetInLayout(rightColLayout, holdWidget,    nhold);
    replaceWidgetInLayout(rightColLayout, nextWidget,    nnext);
    replaceWidgetInLayout(rightColLayout, energyWidget,  nenergy);
}


void GameScreen::setupSpectatorUI(const SpectatorState &ss)
{
    //qWarning() << "Entering spectator mode";
    clearLayout(leftColLayout);
    clearLayout(middleColLayout);
    clearLayout(rightColLayout);

    // Titre principal en haut (milieu)
    QLabel *title = new QLabel("SPECTATING : " + QString::fromStdString(ss.playerUsername), this);
    title->setStyleSheet("color: rgb(0,225,255); font-size:24px; font-weight:bold;");
    title->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    title->setFixedHeight(50);
    title->setMaximumWidth(500);
    applyDropShadow(title);

    // Board
    auto *nbrd = renderBoard(ss.playerGrid, false, ss.isGameOver, this);

    // Hold / Next
    auto *hold   = renderBox("HOLD", renderPiece(ss.holdTetro, 4, 4, this), this);
    auto *next   = renderBox("NEXT", renderPiece(ss.nextTetro, 4, 4, this), this);

    // Noms des joueurs et flèches
    QLabel *prevLabel = new QLabel(" ← Previous Opponent", this);
    QLabel *nextLabel = new QLabel("Next Opponent → ", this);

    prevLabel->setStyleSheet("color: rgb(0,225,255); font-size:24px; font-weight:bold;");
    nextLabel->setStyleSheet("color: rgb(0,225,255); font-size:24px; font-weight:bold;");
    prevLabel->setAlignment(Qt::AlignCenter);
    nextLabel->setAlignment(Qt::AlignCenter);
    applyDropShadow(prevLabel);
    applyDropShadow(nextLabel);

    // Exemple : remplacez ceci par les vrais noms si disponibles dans SpectatorState
    QLabel *prevUsername = new QLabel("O", this);
    QLabel *nextUsername = new QLabel("P", this);
    prevUsername->setAlignment(Qt::AlignCenter);
    nextUsername->setAlignment(Qt::AlignCenter);
    prevUsername->setStyleSheet("color: lightgray;");
    nextUsername->setStyleSheet("color: lightgray;");

    // LEFT: Previous player + arrow + next piece
    leftColLayout->addWidget(prevLabel);
    leftColLayout->addWidget(prevUsername);
    leftColLayout->addSpacerItem(new QSpacerItem(0, 20));
    leftColLayout->addWidget(hold);
    leftColLayout->setContentsMargins(170, 0, 90, 170);

    // MIDDLE: Title + board
    middleColLayout->addWidget(title);
    middleColLayout->addSpacerItem(new QSpacerItem(0, 20));
    middleColLayout->addWidget(nbrd);

    // RIGHT: Hold + arrow + next player
    rightColLayout->addWidget(nextLabel);
    rightColLayout->addWidget(nextUsername);
    rightColLayout->addSpacerItem(new QSpacerItem(0, 20));
    rightColLayout->addWidget(next);
    rightColLayout->setContentsMargins(0, 0, 290, 90);

    // Exit hint
    QLabel *exitHint = new QLabel("Press ESC to exit spectator mode", this);
    exitHint->setStyleSheet("color: gray; font-style: italic;");
    exitHint->setAlignment(Qt::AlignCenter);
    exitHint->setFixedHeight(50);
    rightColLayout->addSpacerItem(new QSpacerItem(0, 20));
    rightColLayout->addWidget(exitHint);
}


void GameScreen::clearLayout(QLayout *layout)
{
    while (auto *item = layout->takeAt(0)) {
        if (auto *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void GameScreen::onEscapePressed()
{
    
    StatusCode fetchRc = session.fetchPlayerData();
    if (fetchRc != StatusCode::SUCCESS) {
        QMessageBox::warning(this,
                             "Erreur réseau",
                             "Impossible de charger les données du joueur avant de quitter.");
    }

    ClientStatus currentStatus = session.getOwnStatus();
    if (currentStatus == ClientStatus::IN_GAME) {
        PlayerState state = session.getPlayerState();

        if (state.gameMode == GameMode::ENDLESS) {
            int currentScore = state.playerScore;
            int bestScore    = session.getBestScore();
            if (currentScore > bestScore) {
                StatusCode r = session.postScore(currentScore);
                if (r != StatusCode::SUCCESS) {
                    QMessageBox::warning(this,
                                         "Error",
                                         "Failed to update high score");
                } else {
                    session.setBestScore(currentScore);
                }
            }
        }

        StatusCode leaveR = session.leaveGame();
        if (leaveR != StatusCode::SUCCESS) {
            QMessageBox::warning(this,
                                 "Error",
                                 "Failed to leave game on server");
            return;
        }
    }
    MainMenu *mainMenu = new MainMenu(session);
    mainMenu->showMaximized();

    this->hide();
}

void GameScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap bg(bgPath);
    QPixmap scaled = bg.scaled(size(), Qt::KeepAspectRatioByExpanding);
    painter.drawPixmap((width()-scaled.width())/2,
                       (height()-scaled.height())/2,
                       scaled);
    QWidget::paintEvent(event);
}
