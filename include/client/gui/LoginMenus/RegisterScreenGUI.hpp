#ifndef REGISTERSCREENGUI_HPP
#define REGISTERSCREENGUI_HPP

#include <QtWidgets>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QFontDatabase>
#include <QMessageBox>

#include "LoginScreenGUI.hpp"
#include "ClientSession.hpp"
#include "Common.hpp"


class RegisterScreen : public QWidget {
    Q_OBJECT
public:
    explicit RegisterScreen(ClientSession &session, QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    ClientSession &session;
    QLineEdit *username;
    QLineEdit *password;
    QLineEdit *confirmedPassword;
    QPushButton *registerButton;
    QPushButton *backToLoginButton;
private slots:
    void openLoginScreen();
    void registerUser();
};

#endif