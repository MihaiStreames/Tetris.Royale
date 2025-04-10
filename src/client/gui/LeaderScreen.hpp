#ifndef LEADERSCREEN_HPP
#define LEADERSCREEN_HPP

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
    explicit LeaderScreen(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif