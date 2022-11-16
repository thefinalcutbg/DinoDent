#include "ObturationWidget.h"

ObturationWidget::ObturationWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	for (auto code : VitaColor::vitaClassic)
	{
		ui.combo_color->addItem(code.data());
	}

	for (auto& str : RestorationMaterial::getNames())
	{
		ui.materialCombo->addItem(str);
	}
}

ObturationWidget::~ObturationWidget()
{
}

ObturationData ObturationWidget::getData()
{
	return ObturationData{
		.color = ui.combo_color->currentIndex(),
		.material = ui.materialCombo->currentIndex()
	};
}

void ObturationWidget::setData(const ObturationData& data)
{
	ui.combo_color->setCurrentIndex(data.color.getIndex());
	ui.materialCombo->setCurrentIndex(data.material.getIndex());
}
