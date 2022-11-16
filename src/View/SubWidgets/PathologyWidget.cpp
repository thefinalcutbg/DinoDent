#include "PathologyWidget.h"

PathologyWidget::PathologyWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void PathologyWidget::setData(const Pathology& data)
{
	ui.pathologyCombo->clear();
	
	auto& date = data.date_diagnosed;

	ui.dateLabel->setText(QString::fromStdString(data.date_diagnosed.toBgStandard()));

	auto list = data.getDiagnosisList();

	for (auto& diagnosis : list)
	{
		ui.pathologyCombo->addItem(QString::fromStdString(diagnosis));
	}
	
	ui.pathologyCombo->setCurrentIndex(data.getDiagnosisIdx());

}

int PathologyWidget::getData()
{
	return ui.pathologyCombo->currentIndex();
}

PathologyWidget::~PathologyWidget()
{
}
