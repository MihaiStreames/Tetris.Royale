#ifndef REGISTERSCREEN_HPP
#define REGISTERSCREEN_HPP

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
#include <QMessageBox>

#include "LoginScreen.hpp"
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