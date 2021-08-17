#pragma once

#include <QWidget>
#include "ui_VitaWidget.h"
#include <array>
#include <type_traits>

class VitaWidget : public QWidget
{
    Q_OBJECT
private:
    static constexpr int lightnessCount{ 6 };

    static inline std::array<int, lightnessCount> lightnessMin{ 17, 20, 22, 29, 36,43 };
    static inline std::array<int, lightnessCount> lightnessMax{ 19, 21, 28, 35, 52, 54 };
    static inline std::array<QString, 3> idx0_5{ "M1", "M2", "M3" }; //lightness 0 and 5;
    static inline std::array<QString, 2> idx1{ "M2", "M3" };         //lightness 1
    static inline std::array<QString, 7> idx2_3_4{ "L1,5", "L2,5", "M1", "M2", "M3", "R1,5", "R2,5" }; //ligthness 2, 3, 4

    static inline std::array<QString, 17> vita_classic{ "", "A1", "A2", "A3", "A3.5", "A4",
                                                            "B1", "B2", "B3", "B4",
                                                            "C1", "C2", "C3", "C4",
                                                            "D2", "D3", "D4" };

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

public:
    VitaWidget(QWidget* parent = Q_NULLPTR);
    void setIndex(int index);
    int getIndex();
private:
    Ui::VitaWidget ui;
};
