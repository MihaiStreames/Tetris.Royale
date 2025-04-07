#include "LoginScreen.hpp"

LoginScreen::LoginScreen(QWidget *parent) : QWidget(parent){

    // Create main layout
    setStyleSheet("background-color:rgb(30, 38, 63); color:rgb(0, 238, 32);");

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
    QFont fontTitle("Andale Mono", 80);
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(fontTitle);
    title->setStyleSheet("border: 2px solid rgb(0, 238, 32); border-radius: 7px; padding: 7px;");
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
    
    titleContainer->setStyleSheet("border: 2px solid rgb(0,238,32); padding: 10px; border-radius: 7px;");

    mainLayout->addWidget(titleContainer);

    // Add buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setSpacing(10);

    loginButton = new QPushButton("Login", this);
    registerButton = new QPushButton("Register", this);
    exitButton = new QPushButton("Exit", this);

    QString buttonStyle = R"(
        QPushButton {
            background-color:rgb(47, 59, 97);
            border: 2px solid rgb(0, 255, 21);
            border-radius: 5px;
            padding: 8px;
        }

        QPushButton:hover {
            background-color:rgb(29, 30, 58);
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