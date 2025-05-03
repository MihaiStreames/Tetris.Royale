
#include "GameRenderGUI.hpp"


QColor colorForValue(int value) {
    switch (static_cast<PieceType>(value)) {
    case PieceType::I:     return QColor(Qt::yellow);
    case PieceType::O:     return QColor(255, 215, 0); // LightGoldenrod1
    case PieceType::J:     return QColor(Qt::green);
    case PieceType::Z:     return QColor(Qt::magenta);
    case PieceType::S:     return QColor(Qt::red);
    case PieceType::T:     return QColor(Qt::cyan);
    case PieceType::L:     return QColor(Qt::blue);
    case PieceType::Single: return QColor(Qt::white);
    default:               return QColor(30, 38, 63);
    }
}

QWidget* renderBoard(const tetroMat &board, bool isOpponentBoard, bool isGameOver, QWidget *parent) {
    // 1) Conteneur avec mise en page verticale
    QWidget    *container  = new QWidget(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(0,0,0,0);

    // 2) Étiquette de titre
    QLabel *titleLabel = new QLabel(isOpponentBoard ? "OPPONENT" : "BOARD", container);
    {
        QFont f = titleLabel->font();
        f.setBold(true);
        titleLabel->setFont(f);
        titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

        if (isOpponentBoard) {
            titleLabel->setText("OPPONENT");
            titleLabel->setStyleSheet(R"(
            color: lightgray;
            background-color: rgba(30, 30, 30, 180);
            border: 2px dashed rgb(0, 225, 255);
            border-radius: 8px;
            padding: 0 5px;
            font-size: 16px;
        )");
            titleLabel->setFixedHeight(30);
            titleLabel->setMaximumWidth(300);
        } else {
            titleLabel->setText("BOARD");
            titleLabel->setStyleSheet(R"(
            color: white;
            background-color: rgba(0, 0, 0, 150);
            border: 2px solid rgb(0, 225, 255);
            border-radius: 10px;
            padding: 0 5px;
            font-size: 20px;
        )");
            titleLabel->setFixedHeight(40);
            titleLabel->setMaximumWidth(500);
        }
    }
    mainLayout->addWidget(titleLabel);

    // 4) Superposition facultative “GAME OVER” tout en haut
    if (isGameOver) {
        QLabel *goLabel = new QLabel("GAME OVER", container);
        QFont f = goLabel->font();
        f.setBold(true);
        f.setPointSize(24);
        goLabel->setFont(f);
        goLabel->setAlignment(Qt::AlignCenter);
        goLabel->setStyleSheet("color: red;");
        if (isOpponentBoard) {
            goLabel->setMaximumWidth(300);
        } else {
            goLabel->setMaximumWidth(500);
        }
        mainLayout->insertWidget(1, goLabel);
    }

    // Vérification si le board est vide (pour l'opponent)
    if (isOpponentBoard && board.empty()) {
        QLabel *noOpponentLabel = new QLabel("NO OPPONENT", container);
        noOpponentLabel->setAlignment(Qt::AlignCenter);
        noOpponentLabel->setStyleSheet(R"(
            color: rgb(0, 225, 255);
            background-color: rgba(30, 30, 30, 180);
            border: 2px dashed rgb(0, 225, 255);
            border-radius: 8px;
            padding: 15px;
            font-size: 25px;
            font-style: italic;
        )");
        noOpponentLabel->setFixedSize(300, 400);
        mainLayout->addWidget(noOpponentLabel);
        return container;
    }


    // 3) Boîte du plateau : utiliser directement un QGridLayout dans le QGroupBox
    QGroupBox *boardBox = new QGroupBox(container);
    if (isOpponentBoard) {
        boardBox->setMaximumSize(300, 400); // plus petit
    } else {
        boardBox->setMaximumSize(500, 800); // plus grand
    }
    boardBox->setTitle("");
    boardBox->setStyleSheet(R"(
        QGroupBox {
            border: 2px solid rgb(0, 225, 255);
            border-radius: 5px;
            margin-top: 10px;
            background-color: rgba(0, 0, 0, 150);
        }
    )");


    // grille à l’intérieur de la boîte
    QGridLayout *grid = new QGridLayout(boardBox);
    grid->setSpacing(0);
    grid->setContentsMargins(0,0,0,0);

    int rows = static_cast<int>(board.size());
    int cols = static_cast<int>(board.empty() ? 0 : board[0].size());
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            QLabel *cell = new QLabel(boardBox);
            cell->setMargin(0);
            cell->setContentsMargins(0, 0, 0, 0);
            cell->setAlignment(Qt::AlignCenter);
            cell->setText("");
            // Si la cellule est activée (valeur non nulle), on lui applique une couleur de fond
            if (board[y][x] != 0) {
                cell->setStyleSheet(QString("background-color: %1; margin: 0; padding: 0;"
                                            "border: 2px solid rgb(0, 180, 200);"
                                            "border-radius: 5px;")
                                        .arg(colorForValue(board[y][x]).name()));
            } else {
                // Cellule inactive : fond transparent
                cell->setStyleSheet("background-color: transparent; margin: 0; padding: 0;");
            }
            grid->addWidget(cell, y, x);
        }
    }
    mainLayout->addWidget(boardBox);

    return container;
}

QWidget* renderEnergyBar(int energy, int maxEnergy, QWidget *parent) {
    QWidget *container = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(4);

    QLabel *textLabel = new QLabel(
        QString("Energy: %1/%2").arg(energy).arg(maxEnergy),
        container
    );
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setStyleSheet("color: white; font-weight: bold;");
    layout->addWidget(textLabel);

    QProgressBar *bar = new QProgressBar(container);
    bar->setRange(0, maxEnergy);
    bar->setValue(energy);
    bar->setTextVisible(false);
    bar->setFixedHeight(20);
    bar->setStyleSheet(R"(
        QProgressBar {
            border: 2px solid rgb(0,225,255);
            border-radius: 10px;
            background-color: rgba(0,0,0,150);
        }
        QProgressBar::chunk {
            background-color: rgba(0,225,255,200);
            border-radius: 8px;
        }
    )");
    layout->addWidget(bar);

    return container;
}

QWidget* renderPiece(PieceType type, int gridHeight, int gridWidth, QWidget *parent) {
    QWidget *container = new QWidget(parent);
    QGridLayout *grid = new QGridLayout(container);
    grid->setSpacing(1);
    grid->setContentsMargins(0,0,0,0);

    std::vector<std::vector<bool>> canvas(gridHeight, std::vector<bool>(gridWidth, false));
    // Logic similaire à placePieceInBoard mais dans une grille bool
    switch (type) {
        case PieceType::I: if (gridHeight>=4 && gridWidth>=4) for(int i=0;i<4;++i) canvas[1][i]=true; break;
        case PieceType::O: if (gridHeight>=2 && gridWidth>=2) canvas[0][0]=canvas[0][1]=canvas[1][0]=canvas[1][1]=true; break;
        case PieceType::T: if (gridHeight>=3 && gridWidth>=3) { canvas[0][1]=true; canvas[1][0]=canvas[1][1]=canvas[1][2]=true; } break;
        case PieceType::S: if (gridHeight>=3 && gridWidth>=3) { canvas[0][1]=canvas[0][2]=canvas[1][0]=canvas[1][1]=true; } break;
        case PieceType::Z: if (gridHeight>=3 && gridWidth>=3) { canvas[0][0]=canvas[0][1]=canvas[1][1]=canvas[1][2]=true; } break;
        case PieceType::J: if (gridHeight>=3 && gridWidth>=3) { canvas[0][0]=true; canvas[1][0]=canvas[1][1]=canvas[1][2]=true; } break;
        case PieceType::L: if (gridHeight>=3 && gridWidth>=3) { canvas[0][2]=true; canvas[1][0]=canvas[1][1]=canvas[1][2]=true; } break;
        default: break;
    }
    for (int y=0; y<gridHeight; ++y) for (int x=0; x<gridWidth; ++x) {
        QLabel *cell = new QLabel(container);
        cell->setMargin(0);
        cell->setContentsMargins(0,0,0,0);
        cell->setAlignment(Qt::AlignCenter);
        if (canvas[y][x]) {
            cell->setStyleSheet(QString("background-color: %1; margin:0; padding:0;"
                                       "border: 2px solid rgb(0, 180, 200);"
                                       "border-radius: 5px;")
                                .arg(colorForValue(static_cast<int>(type)).name()));
        } else {
            cell->setStyleSheet("background-color: transparent; margin:0; padding:0;");
        }
        grid->addWidget(cell, y, x);
    }
    return container;
}

QWidget* renderStats(int score, int level, int linesCleared, QWidget *parent) {
    QWidget *container = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(4);

    const QString labelStyle = R"(
        color: white;
        font-weight: bold;
        font-size: 16px;
        padding: 4px 0;
        border-bottom: 1px solid rgba(0,225,255,100);
    )";

    QLabel *scoreLabel = new QLabel(QString("Score: %1").arg(score), container);
    scoreLabel->setStyleSheet(labelStyle);
    layout->addWidget(scoreLabel);

    QLabel *levelLabel = new QLabel(QString("Level: %1").arg(level), container);
    levelLabel->setStyleSheet(labelStyle);
    layout->addWidget(levelLabel);

    QLabel *linesLabel = new QLabel(QString("Lines: %1").arg(linesCleared), container);
    linesLabel->setStyleSheet(R"(
        color: white;
        font-weight: bold;
        font-size: 16px;
        padding: 4px 0;
    )");
    layout->addWidget(linesLabel);

    return container;
}

QWidget* renderControls(Config &config, QWidget *parent) {
    QWidget *container = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(4);

    const QString lineStyle = R"(
        color: white;
        font-weight: bold;
        font-size: 16px;
        padding: 4px 0;
        border-bottom: 1px solid rgba(0,225,255,100);
    )";
    auto makeLabel = [&](const QString &text) {
        QLabel *lbl = new QLabel(text, container);
        lbl->setStyleSheet(lineStyle);
        return lbl;
    };
    layout->addWidget(makeLabel(QString::fromStdString(config.get("MoveLeft")) + "/" + QString::fromStdString(config.get("MoveRight")) + ": Move"));
    layout->addWidget(makeLabel(QString::fromStdString(config.get("MoveDown")) + "/↓ : Down"));
    layout->addWidget(makeLabel(QString::fromStdString(config.get("RotateLeft")) + "/" + QString::fromStdString(config.get("RotateRight")) + ": Rotate"));
    layout->addWidget(makeLabel(QString::fromStdString(config.get("InstantFall")) + "/Space : Drop"));
    layout->addWidget(makeLabel(QString::fromStdString(config.get("UseBag")) + ": Bag"));
    layout->addWidget(makeLabel(QString::fromStdString(config.get("UseBonus")) + ": Bonus"));
    layout->addWidget(makeLabel(QString::fromStdString(config.get("UseMalus")) + ": Malus"));
    layout->addWidget(makeLabel("Esc: Exit"));

    if (auto *last = container->findChildren<QLabel*>().last()) {
        last->setStyleSheet("color: white; font-weight: bold; font-size: 16px; padding: 4px 0;");
    }
    return container;
}

QWidget* renderBox(const QString &title, QWidget *content, QWidget *parent) {
    QGroupBox *box = new QGroupBox(title, parent);
    QFont font = box->font(); font.setBold(true); box->setFont(font);
    box->setStyleSheet(R"(
        QGroupBox {
            color: white;
            border: 2px solid rgb(0, 225, 255);
            border-radius: 10px;
            margin-top: 10px;
            background-color: rgba(0, 0, 0, 150);
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            font-size: 20px;
        }
    )");
    box->setFixedWidth(250);
    QVBoxLayout *layout = new QVBoxLayout(box);
    layout->addWidget(content);
    return box;
}

void placePieceInBoard(PieceType type, int x, int y, tetroMat &board) {
    static const std::unordered_map<PieceType, std::vector<std::pair<int,int>>> offsetsMap = {
        {PieceType::I, {{0,0},{0,1},{0,2},{0,3}}},
        {PieceType::O, {{0,0},{0,1},{1,0},{1,1}}},
        {PieceType::T, {{0,0},{0,1},{0,2},{1,1}}},
        {PieceType::J, {{0,0},{1,0},{1,1},{1,2}}},
        {PieceType::L, {{0,2},{1,0},{1,1},{1,2}}},
        {PieceType::S, {{1,0},{1,1},{0,1},{0,2}}},
        {PieceType::Z, {{0,0},{0,1},{1,1},{1,2}}}
    };
    auto it = offsetsMap.find(type);
    if (it == offsetsMap.end()) return;
    for (auto [dy, dx] : it->second) {
        int newY = y + dy;
        int newX = x + dx;
        if (newY >= 0 && newY < static_cast<int>(board.size()) && newX >= 0 && newX < static_cast<int>(board[0].size()))

            board[newY][newX] = static_cast<int>(type);
    }
}

