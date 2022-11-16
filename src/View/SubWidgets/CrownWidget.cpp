#include "CrownWidget.h"

CrownWidget::CrownWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (auto& str : CrownPreparation::getNames()) ui.prepCombo->addItem(str);
	for (auto& str : CrownMaterial::getNames()) ui.materialCombo->addItem(str);

}

CrownWidget::~CrownWidget()
{
}

void CrownWidget::setData(const CrownData& data)
{
	ui.prepCombo->setCurrentIndex(data.prep.getIndex());
	ui.vitaWidget->setIndex(data.color.getIndex());
	ui.materialCombo->setCurrentIndex(data.material.getIndex());
}

CrownData CrownWidget::getData()
{
	return CrownData{
		 .material = ui.materialCombo->currentIndex(),
		 .prep = ui.prepCombo->currentIndex(),
		 .color = ui.vitaWidget->getIndex()
	};
}
