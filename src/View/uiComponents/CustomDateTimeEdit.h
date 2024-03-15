#pragma once

#include <QDateTimeEdit>

class CustomDateTimeEdit : public QDateTimeEdit
{
    Q_OBJECT

public:

    CustomDateTimeEdit(QWidget* parent = nullptr);
    ~CustomDateTimeEdit();

};
