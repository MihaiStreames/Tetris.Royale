
#include "LeaderScreen.hpp"


LeaderScreen::LeaderScreen(ClientSession &session, QWidget *parent) : QWidget(parent), session(session){

    // Create main layout
    QFontDatabase::addApplicationFont("src/client/gui/resources/orbitron.ttf");
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // Title
    QWidget *titleContainer = new QWidget(this);
    QVBoxLayout *titleLayout = new QVBoxLayout(titleContainer);
    titleLayout->setAlignment(Qt::AlignTop);
    titleLayout->setSpacing(10);

    QLabel *title = new QLabel("Leaderboard", titleContainer);
    QFont fontTitle("Orbitron", 50);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(fontTitle);
    title->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 7px; padding: 7px;");
    titleLayout->addWidget(title);
    
    // Create the table and headers 
    QTableWidget *table = new QTableWidget(0, 2, this);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Player"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("High Score"));

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table->setStyleSheet(
        "QTableWidget { "
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(19, 26, 64, 90), stop:1 rgba(189, 248, 255, 90));"
        "   color: rgb(202, 237, 241);"
        "   gridline-color: rgb(0, 255, 255);"
        "   font: 16px 'Orbitron';"
        "}"
    );

    table->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: rgb(0, 255, 255);"
        "   color: rgb(0, 60, 129);"
        "   font: 19px 'Orbitron';"
        "   padding: 5px;"
        "}"
    );

    table->verticalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: rgb(0, 255, 255);"
        "   color: rgb(0, 60, 129);"
        "   font: 19px 'Orbitron';"
        "   padding: 5px;"
        "}"
    );

    // Show leaderboard data
    std::vector<PlayerScore> leaderboard = session.getLeaderboard(10);
    for (int i = 0; i < static_cast<int>(leaderboard.size()); ++i) {
        const auto &player = leaderboard[i];
    
        table->insertRow(i);
    
        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(player.name)));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(player.score)));
    }
    


    mainLayout->addWidget(titleContainer);
    mainLayout->addWidget(table);

    setLayout(mainLayout);

    setWindowTitle("Leaderboard");
}

void LeaderScreen::paintEvent(QPaintEvent *event) {
    // Paints the background

    QPainter painter(this);
    QPixmap screenPixmap("src/client/gui/resources/tetris_main.png");

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}

