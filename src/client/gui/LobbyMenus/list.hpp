#ifndef LIST_HPP
#define LIST_HPP

#include <QListWidget>
#include <vector>
#include <QString>

template<typename ParentType, typename LayoutType>
QListWidget* createList(ParentType parent, LayoutType layout) {
    QListWidget* lst = new QListWidget(parent);
    layout->addWidget(lst);
    return lst;
}

#endif // LIST_H
