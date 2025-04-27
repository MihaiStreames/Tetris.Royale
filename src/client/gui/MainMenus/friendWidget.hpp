#ifndef FRIENDWIDGET_HPP
#define FRIENDWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>



class FriendWidget : public QWidget {
    Q_OBJECT

public:
    enum State{
        Online,
        InLobby,
        InGame,
        Offline
    };


    enum Context {
        FriendsList,
        LobbyInvites,
        FriendRequest
    };

    QString buttonStyle = R"(
        QPushButton {
            font-family: 'Orbitron', sans-serif;
            font-size: 14px;
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 88, 219, 150), stop:1 rgba(31, 203, 255, 150));
            color: white;
            border: 2px solid rgb(0, 225, 255);
            border-radius: 10px;
            padding: 10px 20px;
        }
        QPushButton:hover {
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 64, 182, 150), stop:1 rgba(0, 155, 226, 150));
        }
    )";

    explicit FriendWidget(const QString &friendName, Context context,State state, QWidget *parent = nullptr);

    QString getFriendName() const { return friendName; }

signals:
    void firstButtonClicked(const QString &friendName); 
    void secondButtonClicked(const QString &friendName); 
    void thirdButtonClicked(const QString &friendName); 
    void fourthButtonClicked(const QString &friendName); 

private:
    Context context;
    QString friendName;
    QPushButton *mainButton;
    QPushButton *firstOptionButton;
    QPushButton *secondOptionButton;
    QPushButton *thirdOptionButton;
    QPushButton *fourthOptionButton;

    QLabel *stateCircle;


    void setupLayout(Context context);
    void updateState(State state);
};

#endif // FRIENDWIDGET_HPP