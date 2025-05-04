#ifndef LEADERSCREENGUI_HPP
#define LEADERSCREENGUI_HPP

#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QWidget>
#include <QPainter>
#include <QFontDatabase>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "Common.hpp"
#include "ClientSession.hpp"


class LeaderScreen : public QWidget {
    Q_OBJECT 
public:
    explicit LeaderScreen(ClientSession &session, QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    ClientSession &session;
};

#endif