#include "VitaWidget.h"


VitaWidget::VitaWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    set3DMaster(0);

    ui.radioClassic->setChecked(true);

    connect(ui.radioClassic, &QRadioButton::toggled, [=] { set3DMaster(false); });
    connect(ui.radioMaster, &QRadioButton::toggled, [=] {set3DMaster(true); });
    connect(ui.lightness, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {switchColor(index); });

}

void VitaWidget::set3DMaster(bool vita_3d)
{
    ui.c_label->setVisible(vita_3d);
    ui.l_label->setVisible(vita_3d);
    ui.lightness->setVisible(vita_3d);
    ui.chroma_hue->setVisible(vita_3d);

    ui.index->setVisible(!vita_3d);
    ui.idx_label->setVisible(!vita_3d);
}

void VitaWidget::switchColor(int index)
{
    if (!index)
    {
        ui.chroma_hue->clear();
        return;
    }

    switch (--index)
    {
    case 0: case 5:
        changeColorContents(idx0_5);
        break;
    case 1:
        changeColorContents(idx1);
        break;
    case 2: case 3: case 4:
        changeColorContents(idx2_3_4);
        break;
    }
}


void VitaWidget::setIndex(int index)
{
    if (index < -1 || index > 45)//checking for valid index
    {
        setIndex(0);
        return;
    }

    if (index < 17) //Vita classic
    {
        ui.index->setCurrentIndex(index);
        ui.radioClassic->setChecked(true);
        return;
    }

    //Vita 3D Master

    ui.radioMaster->setChecked(true);

    for (int i = 0; i < lightnessCount; i++) //Vita 3D Master
    {
        if (index <= lightnessMax[i])
        {
            ui.lightness->setCurrentIndex(i + 1); //index 0 is invalid;
            ui.chroma_hue->setCurrentIndex(index - lightnessMin[i]);

            return;
        }
    }
}

int VitaWidget::getIndex()
{

    if (ui.radioClassic->isChecked()) return ui.index->currentIndex(); //vita classic

    if (ui.lightness->currentIndex() == 0) return 0; //3d master non-set

    return lightnessMin[ui.lightness->currentIndex()-1] + ui.chroma_hue->currentIndex(); 
}
