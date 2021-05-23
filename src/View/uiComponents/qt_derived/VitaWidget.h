#pragma once

#include <QWidget>
#include "ui_VitaWidget.h"
#include <array>
#include <type_traits>

class VitaWidget : public QWidget
{
    Q_OBJECT

    std::array<QString, 3> idx0_5{ "M1", "M2", "M3" };
    std::array<QString, 2> idx1{ "M2", "M3" };
    std::array<QString, 7> idx2_3_4{ "L1,5", "L2,5", "M1", "M2", "M3", "R1,5", "R2,5" };

    std::array<int, 8> jaggedLength{ 0, 1, 4, 6, 13, 20, 27, 30 };

    void switchColor(int index);

    // std::array<QString, T>
    template<typename Items, int Size = std::tuple_size<Items>::value>
    typename std::enable_if<(Size > 0) && std::is_same<typename Items::value_type, QString>::value>::type changeColorContents(const Items &items)
    {
        ui.chroma_hue->clear();
        for (int i = 0; i < Size; i++)
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
