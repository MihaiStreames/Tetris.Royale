#ifndef LABEL_HPP
#define LABEL_HPP

#include <QString>
#include <QLabel>


template<typename LayoutType, typename ParentType>

QLabel* createLabel(const QString& name, LayoutType* layout, ParentType* parent) {

    QLabel* label = new QLabel(name, parent);
    layout->addWidget(label);
    return label;

}


#endif // LABEL_H
