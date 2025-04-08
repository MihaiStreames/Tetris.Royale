#include "LoginScreen.hpp"

LoginScreen::LoginScreen(QWidget *parent) : QWidget(parent){

    // Create main layout
    int orbitronFont = QFontDatabase::addApplicationFont(":/fonts/orbitron.ttf");
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // Title
    QWidget *titleContainer = new QWidget(this);
    QVBoxLayout *titleInputLayout = new QVBoxLayout(titleContainer);
    titleInputLayout->setAlignment(Qt::AlignTop);
    titleInputLayout->setSpacing(10);

    QLabel *title = new QLabel("TETRIS ROYALE", titleContainer);
    QFont fontTitle("Orbitron", 80);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(fontTitle);
    title->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 7px; padding: 7px;");
    titleInputLayout->addWidget(title);
    
    // Username and password input boxes
    QFont fontInteractions("Andale Mono", 26);
    username = new QLineEdit(this);
    username->setPlaceholderText("Username");
    username->setFont(fontInteractions);

    password = new QLineEdit(this);
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    password->setFont(fontInteractions);

    titleInputLayout->addWidget(username);
    titleInputLayout->addWidget(password);
    
    titleContainer->setStyleSheet("border: 2px solid rgb(0,255,255); padding: 10px; border-radius: 7px;");

    mainLayout->addWidget(titleContainer);

    // Add buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setSpacing(10);

    loginButton = new QPushButton("Login", this);
    registerButton = new QPushButton("Register", this);
    exitButton = new QPushButton("Exit", this);

    QString buttonStyle = R"(
        QPushButton {
            font-family: 'Orbitron', sans-serif;
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 88, 219, 150), stop:1 rgba(31, 203, 255, 150));
            color: white;
            border: 2px solid rgb(0, 225, 255);
            border-radius: 10px;
            padding: 10px 20px;
        }

        QPushButton:hover {
            background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 64, 182, 150), stop:1 rgba(0, 155, 226, 150));
        }
    )";

    loginButton->setStyleSheet(buttonStyle);
    registerButton->setStyleSheet(buttonStyle);
    exitButton->setStyleSheet(buttonStyle);

    loginButton->setFont(fontInteractions);
    registerButton->setFont(fontInteractions);
    exitButton->setFont(fontInteractions);

    buttonsLayout->addWidget(loginButton);
    buttonsLayout->addWidget(registerButton);
    buttonsLayout->addWidget(exitButton);

    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    setWindowTitle("Login Screen");
}

void LoginScreen::paintEvent(QPaintEvent *event) {
    // Paints the background

    QPainter painter(this);
    QPixmap screenPixmap("./tetris_main.png");

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}