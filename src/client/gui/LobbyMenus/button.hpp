#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <QPushButton>
#include <QString>
#include <QLayout>
#include <QObject>

template<typename LayoutType, typename ParentType, typename ReceiverType>
QPushButton* createButton(const QString& name, LayoutType* layout, ParentType* parent, void (ReceiverType::*slot)(), int x=50, int y=50, int lenght=150, int width=45) {
    QPushButton *btn = new QPushButton(name, parent);

    layout->addWidget(btn);

    QObject::connect(btn, &QPushButton::clicked, parent, slot);

    return btn;
}

#endif // BUTTON_H
