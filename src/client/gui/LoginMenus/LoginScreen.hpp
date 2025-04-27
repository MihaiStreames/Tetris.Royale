#ifndef LOGINSCREEN_HPP
#define LOGINSCREEN_HPP

#pragma once

#include "ClientSession.hpp"
#include "../MainMenus/mainMenu.hpp"
#include "RegisterScreen.hpp"

#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QFontDatabase>

class LoginScreen : public QWidget {
    Q_OBJECT
public:
    explicit LoginScreen(ClientSession &session, QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    ClientSession &session;
    QLineEdit *username;
    QLineEdit *password;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *exitButton;
private slots:
    void openRegisterScreen();
    void loginUser();
    void exitScreen();
};

#endif