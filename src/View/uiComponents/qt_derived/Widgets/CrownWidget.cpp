#include "CrownWidget.h"

CrownWidget::CrownWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (auto str : CrownData::prepTypes()) ui.comboBox->addItem(str.data());

}

CrownWidget::~CrownWidget()
{
}

void CrownWidget::setData(const CrownData& data)
{
	ui.comboBox->setCurrentIndex(data.prep_type);
	ui.vitaWidget->setIndex(data.color);
	ui.materialEdit->setText(QString::fromStdString(data.material));
}

CrownData CrownWidget::getData()
{
	return CrownData{
		 ui.materialEdit->text().toStdString(),
		 ui.comboBox->currentIndex(),
		 ui.vitaWidget->getIndex()
	};
}
