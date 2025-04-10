#include "LeaderScreen.hpp"

LeaderScreen::LeaderScreen(QWidget *parent) : QWidget(parent){

    // Create main layout
    QFontDatabase::addApplicationFont(":/fonts/orbitron.ttf");
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

    // Examples
    table->insertRow(0);
    table->insertRow(1);

    table->setItem(0, 0, new QTableWidgetItem("John Doe"));
    table->setItem(0, 1, new QTableWidgetItem("3000"));
    table->setItem(1, 0, new QTableWidgetItem("Jane Doe"));
    table->setItem(1, 1, new QTableWidgetItem("2900"));

    mainLayout->addWidget(titleContainer);
    mainLayout->addWidget(table);

    setLayout(mainLayout);

    setWindowTitle("Leaderboard");
}

void LeaderScreen::paintEvent(QPaintEvent *event) {
    // Paints the background

    QPainter painter(this);
    QPixmap screenPixmap("resources/tetris_main.png");

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}
