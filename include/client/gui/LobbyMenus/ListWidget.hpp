#ifndef LISTWIDGET_HPP
#define LISTWIDGET_HPP

#include <QListWidget>
#include <QString>
#include <vector>


template<typename ParentType, typename LayoutType>

QListWidget* createList(ParentType parent, LayoutType layout) {

    QListWidget* lst = new QListWidget(parent);
    layout->addWidget(lst);
    return lst;

}


#endif // LIST_H
