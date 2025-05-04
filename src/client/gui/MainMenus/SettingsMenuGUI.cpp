
#include "SettingsMenuGUI.hpp"


SettingsScreen::SettingsScreen(MainMenu* mainMenuView, QWidget *parent): QWidget(parent), mainMenu(mainMenuView){

    QString fontPath = QString(TETRIS_FONTS_DIR) + "/orbitron.ttf";
    QFontDatabase::addApplicationFont(fontPath);
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // TITRE
    QWidget *titleContainer = new QWidget(this);
    QVBoxLayout *titleLayout = new QVBoxLayout(titleContainer);
    titleLayout->setAlignment(Qt::AlignTop);
    titleLayout->setSpacing(10);

    // Création du Label
    QLabel *title = new QLabel("CONTROLS", titleContainer);
    QFont fontTitle("Orbitron", 80);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(fontTitle);
    title->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 7px; padding: 7px;");
    titleLayout->addWidget(title);
    
    // Créer le tableau de settings
    QTableWidget *table = new QTableWidget(0, 2, this);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Control"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Key"));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(80); 
    table->setFocusPolicy(Qt::NoFocus);


    // Style
    table->setStyleSheet(
        "QTableWidget { "
        "   background: transparent;"
        "   color: rgb(202, 237, 241);"
        "   font: 22px 'Orbitron';"
        "   border: none;"
        "   gridline-color: black;"
        "}"
        "QTableWidget::item {"
        "   background-color: transparent;"
        "   border: 2px solid rgb(0, 89, 255);"
        "   padding: 8px;"
        "}"
    );

    table->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: rgb(0, 89, 255);"
        "   color: black;"
        "   font: 24px 'Orbitron';"
        "   padding: 10px;"
        "   border: none;"
        "}"
    );

    // Tableau non interactif 
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setAttribute(Qt::WA_TransparentForMouseEvents);


    // Ajout des données
    QStringList controls = {"Left", "Right", "Rotate", "Drop", "Switch Player","Power-up"};
    QStringList keys = {"←", "→", "↑","Space","S","Q" };

    for (int i = 0; i < controls.size(); ++i) {
        table->insertRow(i);
        table->setItem(i, 0, new QTableWidgetItem(controls[i]));
        table->setItem(i, 1, new QTableWidgetItem(keys[i]));
    }
    
    // Back to main menu
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignLeft);


    backToMainButton = new QPushButton("Back to main menu", this);
    backToMainButton->setStyleSheet(buttonStyle);
    backToMainButton->setFixedSize(350, 60);
    connect(backToMainButton, &QPushButton::clicked, this, &SettingsScreen::backToMainMenu);
    
    mainLayout->addWidget(titleContainer);
    mainLayout->addWidget(table);
    mainLayout->addWidget(backToMainButton);

    setLayout(mainLayout);

};

void SettingsScreen::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap screenPixmap(bgPath);

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}

void SettingsScreen::backToMainMenu() {
    mainMenu->setVisible(true);
    mainMenu->showMaximized();
    this->close();
}

