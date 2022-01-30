#include "PathologyWidget.h"

PathologyWidget::PathologyWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void PathologyWidget::setData(const PathologyData& data)
{
	ui.pathologyCombo->clear();
	
	auto& date = data.date_diagnosed;

	ui.dateLabel->setText(QString::fromStdString(data.date_diagnosed.toString()));

	auto list = data.diagnosisList();

	if (list == nullptr) return;

	for (auto& diagnosis : *data.diagnosisList())
	{
		ui.pathologyCombo->addItem(QString::fromStdString(diagnosis));
	}
	
	ui.pathologyCombo->setCurrentIndex(data.diagnosis_index);

}

int PathologyWidget::getData()
{
	return ui.pathologyCombo->currentIndex();
}

PathologyWidget::~PathologyWidget()
{
}
