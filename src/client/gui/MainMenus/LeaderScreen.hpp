#ifndef LEADERSCREEN_HPP
#define LEADERSCREEN_HPP

#include "Common.hpp"
#include "ClientSession.hpp"

#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QPainter>
#include <QFontDatabase>
#include <QTableWidget>
#include <QTableWidgetItem>

class LeaderScreen : public QWidget {
public:
    explicit LeaderScreen(ClientSession &session, QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    ClientSession &session;
};

#endif