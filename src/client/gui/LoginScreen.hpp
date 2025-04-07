#ifndef LOGINSCREEN_HPP
#define LOGINSCREEN_HPP

#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QString>

class LoginScreen : public QWidget {
public:
    explicit LoginScreen(QWidget *parent = nullptr);
private:
    QLineEdit *username;
    QLineEdit *password;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *exitButton;
};

#endif