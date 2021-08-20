#include "VitaWidget.h"



VitaWidget::VitaWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    set3DMaster(false);

    ui.radioClassic->setChecked(true);

    connect(ui.radioClassic, &QRadioButton::toggled, [=] { set3DMaster(false); });
    connect(ui.radioMaster, &QRadioButton::toggled, [=] {set3DMaster(true); });
    connect(ui.index, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {switchColor(index); });

}

void VitaWidget::set3DMaster(bool vita_3d)
{
    ui.c_label->setVisible(vita_3d);
    ui.chroma_hue->setVisible(vita_3d);

    ui.index->clear();

    if (!vita_3d) //Vita Classic appearence
    {
        ui.idx_label->setText(u8"Цвят:");

        for (int i = 0; i < vita_classic.size(); i++)
            ui.index->addItem(vita_classic[i]);

        return;
    }

    //Vita 3D Master appearence

    ui.idx_label->setText(u8"Яркост:");


    ui.index->addItem(QString{});

    for (int i = 0; i < 6; i++)
        ui.index->addItem(QString::number(i));
}

void VitaWidget::switchColor(int index)
{
    if (!index){
        ui.chroma_hue->clear(); 
        return;
    }

    switch (--index) {
        case 0: case 5: changeColorContents(idx0_5); break;
        case 1: changeColorContents(idx1); break;
        case 2: case 3: case 4: changeColorContents(idx2_3_4);  break;
    }
}


void VitaWidget::setIndex(int index)
{
    //checking for valid index
    if (index < -1 || index > 45){
        setIndex(0); 
        return;
    }

    // Vita classic
    if (index < 17) {
        ui.index->setCurrentIndex(index);
        ui.radioClassic->setChecked(true);
        return;
    }

    //Vita 3D Master
    ui.radioMaster->setChecked(true);

    for (int i = 0; i < lightnessCount; i++) //Vita 3D Master
    {
        if (index <= dateMax[i])
        {
            ui.index->setCurrentIndex(i + 1); //index 0 is invalid;
            ui.chroma_hue->setCurrentIndex(index - lightnessMin[i]);

            return;
        }
    }
}

int VitaWidget::getIndex()
{

    if (ui.radioClassic->isChecked()) return ui.index->currentIndex(); //vita classic

    if (ui.index->currentIndex() == 0) return 0; //3d master non-set

    int idx = ui.index->currentIndex()-1;
    int chroma = ui.chroma_hue->currentIndex();

    return lightnessMin[idx] + chroma; 
}
