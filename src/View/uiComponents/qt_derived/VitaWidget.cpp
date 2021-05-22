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

void VitaWidget::setIndex(int index, bool vita3DMaster)
{
    if (!indexIsValid(index, vita3DMaster)) //checking for valid index
    {
        setIndex(0, vita3DMaster);
        return;
    }


    ui.radioMaster->setChecked(vita3DMaster); //changing appearence
    //ui.radioClassic->setChecked(!vita3DMaster);

    if (!vita3DMaster) //Vita classic
    {
        ui.index->setCurrentIndex(index);
        getIndex();
        return;
    }

    for (int i = 0; i < jaggedLength.size(); i++) //Vita 3D Master
    {
        if (index < jaggedLength[i + 1])
        {
            ui.lightness->setCurrentIndex(i);
            ui.chroma_hue->setCurrentIndex(index - jaggedLength[i]);
            getIndex();
            return;
        }
    }


}

std::tuple<int, bool> VitaWidget::getIndex()
{
    int index = 0;
    bool master{ false };
    if (ui.radioClassic->isChecked())
    {
        index = ui.index->currentIndex();

    }
    else
    {
        index = jaggedLength[ui.lightness->currentIndex()];

        if (index)
            index = index + ui.chroma_hue->currentIndex();

        master = true;
    }

    return std::make_tuple(index, master);
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

bool VitaWidget::indexIsValid(int index, bool vita3dMaster)
{
    return (vita3dMaster) ?

        (index > -1 && index < 30)
        :
        (index > -1 && index < 17);
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


