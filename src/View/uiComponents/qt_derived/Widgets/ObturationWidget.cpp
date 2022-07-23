#include "ObturationWidget.h"



ObturationWidget::ObturationWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	for (auto code : ObturationData::colorStrings())
	{
		ui.combo_color->addItem(code.data());
	}
}

ObturationWidget::~ObturationWidget()
{
}

ObturationData ObturationWidget::getData()
{
	return ObturationData{
		ui.combo_color->currentIndex(),
		ui.materialEdit->text().toStdString()
	};
}

void ObturationWidget::setData(const ObturationData& data)
{
	ui.combo_color->setCurrentIndex(data.color);
	ui.materialEdit->setText(QString::fromStdString(data.material));
}
