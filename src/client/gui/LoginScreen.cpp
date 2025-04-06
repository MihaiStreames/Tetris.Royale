#include "LoginScreen.hpp"

LoginScreen::LoginScreen(QWidget *parent) : QWidget(parent){

    setStyleSheet("background-color:rgb(30, 38, 63); color:rgb(0, 238, 32);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    QFont fontInteractions("Andale Mono", 18);
    username = new QLineEdit(this);
    username->setPlaceholderText("Username");
    username->setFont(fontInteractions);

    password = new QLineEdit(this);
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    password->setFont(fontInteractions);

    QLabel *title = new QLabel("TETRIS ROYALE", this);
    QFont fontTitle("Andale Mono", 48);
    title->setAlignment(Qt::AlignCenter);
    title->setFont(fontTitle);
    title->setStyleSheet("border: 2px solid rgb(0, 238, 32); border-radius: 7px; padding: 7px;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(username);
    mainLayout->addWidget(password);

    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);

    loginButton = new QPushButton("Login", this);
    registerButton = new QPushButton("Register", this);
    exitButton = new QPushButton("Exit", this);

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