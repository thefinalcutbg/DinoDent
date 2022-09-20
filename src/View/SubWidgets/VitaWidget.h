#pragma once

#include <QWidget>
#include "ui_VitaWidget.h"
#include <array>
#include <type_traits>

class VitaWidget : public QWidget
{
    Q_OBJECT
private:

    void switchColor(int index);

    void set3DMaster(bool vita_3d);

public:
    VitaWidget(QWidget* parent = Q_NULLPTR);
    void setIndex(int index);
    int getIndex();
private:
    Ui::VitaWidget ui;
};
