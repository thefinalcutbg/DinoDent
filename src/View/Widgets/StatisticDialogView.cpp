#include "StatisticDialogView.h"
#include "Presenter/StatisticDialogPresenter.h"
#include "QtVersion.h"
StatisticDialogView::StatisticDialogView(StatisticDialogPresenter& p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.procedureCombo, QtComboIndexChanged, [&](int idx) {presenter.setProcedureType(idx);});
	connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed();});

	presenter.setView(this);
}

StatisticDialogView::~StatisticDialogView()
{}

void StatisticDialogView::setNameFilterList(const std::vector<std::string> namesFilter)
{
	ui.nameFilterList->clear();

	for (auto& name : namesFilter)
	{
		QListWidgetItem* item = new QListWidgetItem(name.c_str(), ui.nameFilterList);
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		item->setCheckState(Qt::Unchecked);
		ui.nameFilterList->addItem(item);
	}
}

void StatisticDialogView::setDiagnosisFilterList(const std::vector<std::string> diagnosis)
{
	ui.diagnosisFilterList->clear();

	for (auto& diagnosis : diagnosis)
	{
		QListWidgetItem* item = new QListWidgetItem(diagnosis.c_str(), ui.diagnosisFilterList);
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		item->setCheckState(Qt::Unchecked);
		ui.diagnosisFilterList->addItem(item);
	}
}

void StatisticDialogView::setStatistic(const DentalStatistic& s)
{
}

DentalStatistic StatisticDialogView::getStatistic()
{
	auto getFilterList = [](QGroupBox* group, QListWidget* list) {

		std::vector<std::string> result;

		if (!group->isChecked()) return result;

		for (int i = 0; i < list->count(); i++)
		{
			QListWidgetItem* item = list->item(i);

			if (item->checkState() == Qt::Checked) {
				result.push_back(item->text().toStdString());
			}
		}

		return result;
	};

	return DentalStatistic{
		.name = ui.nameEdit->text().toStdString(),
		.procedureType = ui.procedureCombo->currentIndex(),
		.age = static_cast<DentalStatistic::AgeFilter>(ui.ageCombo->currentIndex()),

		.tooth = ui.ageCombo->isHidden() ?
					DentalStatistic::ToothFilter::All
					:
					static_cast<DentalStatistic::ToothFilter>(ui.toothCombo->currentIndex()),

		.nhifOnly = ui.nhifCheckBox->isChecked(),
		.diagnosisFilter = getFilterList(ui.diagnosisGroup, ui.diagnosisFilterList),
		.procedureNameFilter = getFilterList(ui.nameGroup, ui.nameFilterList)
	};

}

void StatisticDialogView::hideToothFilter(bool hidden)
{
	ui.toothCombo->setHidden(hidden);
	ui.label_4->setHidden(hidden);
}
