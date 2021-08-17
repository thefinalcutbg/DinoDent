#include "ObturationWidget.h"



ObturationWidget::ObturationWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	static std::array<QString, 17> colorCodes
	{
		"",
		"A1", "A2", "A3", "A3,5", "A4",
		"B1", "B2", "B3", "B4",
		"C1", "C2", "C3", "C4",
		"D2", "D3", "D4"
	};


	for (QString code : colorCodes)
	{
		ui.combo_color->addItem(code);
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
