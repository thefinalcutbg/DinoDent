#pragma once

#include <QWidget>
#include "ui_VitaWidget.h"
#include <array>

class VitaWidget : public QWidget
{
    Q_OBJECT

    std::array<QString, 3> idx0_5{ "M1", "M2", "M3" };
    std::array<QString, 2> idx1{ "M2", "M3" };
    std::array<QString, 7> idx2_3_4{ "L1,5", "L2,5", "M1", "M2", "M3", "R1,5", "R2,5" };

    std::array<int, 8> jaggedLength{ 0, 1, 4, 6, 13, 20, 27, 30 };

    void switchColor(int index);

    template<int T>
    void changeColorContents(std::array<QString, T> items)
    {
        ui.chroma_hue->clear();
        for (int i = 0; i < T; i++)
        {
            ui.chroma_hue->addItem(items[i]);
        }
    }

    void set3DMaster(bool vita_3d);

    bool indexIsValid(int index, bool vita3dMaster);

public:
    VitaWidget(QWidget* parent = Q_NULLPTR);
    void setIndex(int index, bool vita3DMaster);
    std::tuple<int, bool> getIndex();
private:
    Ui::VitaWidget ui;
};
