#include "VitaWidget.h"
#include "Model/Dental/VitaColor.h"
#include <QDebug>

VitaWidget::VitaWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    set3DMaster(false);

    ui.radioClassic->setChecked(true);

    connect(ui.radioClassic, &QRadioButton::toggled, [=] { set3DMaster(false); });
    connect(ui.radioMaster, &QRadioButton::toggled, [=] {set3DMaster(true); });
    connect(ui.index, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {switchColor(index); });

    set3DMaster(false);
}

void VitaWidget::set3DMaster(bool vita_3d)
{
    ui.c_label->setVisible(vita_3d);
    ui.chroma_hue->setVisible(vita_3d);

    ui.index->clear();

    if (!vita_3d) //Vita Classic appearence
    {
        ui.idx_label->setText("Цвят:");

        for (auto& string : VitaColor::vitaClassic)
            ui.index->addItem(QString(string.data()));

        return;
    }

    //Vita 3D Master appearence

    ui.idx_label->setText("Яркост:");

    for (auto& string : VitaColor::lightness)
        ui.index->addItem(string.data());

    ui.index->setCurrentIndex(0);
}

void VitaWidget::switchColor(int index)
{
    ui.chroma_hue->clear();

    if (ui.radioClassic->isChecked()) { return; }

    int& lightness = index;

    for (auto& string : VitaColor::getChroma(lightness))
    {
        ui.chroma_hue->addItem(string.data());
    }
}


void VitaWidget::setIndex(int index)
{
    auto var = VitaColor(index).toColor();

    qDebug() << index;

    // Vita classic
    if (var.index() == 0) {

        ui.radioMaster->setChecked(false);
        set3DMaster(false);
        ui.index->setCurrentIndex(index);
        ui.radioClassic->setChecked(true);
        return;
    }

    //Vita 3D Master
    ui.radioMaster->setChecked(true);

    auto vita3d = std::get<Vita3dMaster>(var);

    set3DMaster(true);

    qDebug() << vita3d.m_lightness << vita3d.m_chroma;

    ui.index->setCurrentIndex(vita3d.m_lightness);
    ui.chroma_hue->setCurrentIndex(vita3d.m_chroma);
}

int VitaWidget::getIndex()
{
    
    //vita classic
    if (ui.radioClassic->isChecked()) {
        auto color = VitaColor(ui.index->currentIndex());
        return color.getIndex();
    }
 

    auto color = VitaColor(ui.index->currentIndex(), ui.chroma_hue->currentIndex());

    return color.getIndex();

}
