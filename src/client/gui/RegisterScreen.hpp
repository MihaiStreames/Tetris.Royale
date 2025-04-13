#ifndef REGISTERSCREEN_HPP
#define REGISTERSCREEN_HPP

#include "LoginScreen.hpp"
#include "../../../include/client/connectivity/ClientSession.hpp"
#include "../../../include/client/connectivity/Common.hpp"

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

class RegisterScreen : public QWidget {
public:
    explicit RegisterScreen(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
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