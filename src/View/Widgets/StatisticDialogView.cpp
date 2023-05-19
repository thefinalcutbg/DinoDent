#include "StatisticDialogView.h"
#include "Presenter/StatisticDialogPresenter.h"
#include "QtVersion.h"
#include "Model/Dental/Diagnosis.h"
#include <QPainter>

void StatisticDialogView::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);
}

StatisticDialogView::StatisticDialogView(StatisticDialogPresenter& p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Статистики");
	setWindowIcon(QIcon(":/icons/icon_statistic.png"));

	connect(ui.procedureCombo, QtComboIndexChanged, [&](int idx) {presenter.setProcedureType(idx);});

	for (auto &name : Diagnosis::getNames())
	{
		QListWidgetItem* item = new QListWidgetItem(name.c_str(), ui.diagnosisFilterList);
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		item->setCheckState(Qt::Unchecked);
		ui.diagnosisFilterList->addItem(item);
	}

	ui.yearSpinBox->setValue(Date::currentDate().year - 1);

	presenter.setView(this);

	ui.procedureCombo->setCurrentIndex(2);

	connect(ui.yearSpinBox, &QSpinBox::valueChanged, [&] { presenter.parameterChanged(); });
	connect(ui.ageCombo, QtComboIndexChanged, [&]{ presenter.parameterChanged(); });
	connect(ui.toothCombo, QtComboIndexChanged, [&] { presenter.parameterChanged(); });
	connect(ui.financingCombo, QtComboIndexChanged, [&] { presenter.parameterChanged(); });
	connect(ui.diagnosisGroup, &QGroupBox::clicked, [&] { presenter.parameterChanged(); });
	connect(ui.nameGroup,&QGroupBox::clicked, [&] { presenter.parameterChanged(); });
	connect(ui.nameFilterList, &QListWidget::itemChanged, [&] { presenter.parameterChanged(); });
	connect(ui.diagnosisFilterList, &QListWidget::itemChanged, [&] { presenter.parameterChanged(); });


	presenter.parameterChanged();
}

StatisticDialogView::~StatisticDialogView()
{}

void StatisticDialogView::setProcedureFilter(const std::vector<ProcedureCode> namesFilter)
{
	QSignalBlocker b(ui.nameFilterList);
	QSignalBlocker b2(ui.nameGroup);

	ui.nameFilterList->clear();
	m_procedureCodes.clear();

	for (auto& code : namesFilter)
	{
		QListWidgetItem* item = new QListWidgetItem(code.name().c_str(), ui.nameFilterList);
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		item->setCheckState(Qt::Unchecked);
		ui.nameFilterList->addItem(item);

		m_procedureCodes.push_back(code.code().c_str());
	}

	ui.nameGroup->setChecked(false);

	presenter.parameterChanged();
}

void StatisticDialogView::setStatistic(const DentalStatistic& s)
{
}

DentalStatistic StatisticDialogView::getStatistic()
{

	decltype(DentalStatistic::diagnosisFilter) diagnosisFilter;
	
	if (ui.diagnosisGroup->isChecked())
	{
		for (int i = 0; i < ui.diagnosisFilterList->count(); i++) {
			
			if (ui.diagnosisFilterList->item(i)->checkState() != Qt::Checked) continue;

			diagnosisFilter.push_back(i);
		}
	}

	decltype(DentalStatistic::procedureCodeFilter) procedureCodesFilter;

	if (ui.nameGroup->isChecked())
	{
		for (int i = 0; i < ui.nameFilterList->count(); i++)
		{
			if (ui.nameFilterList->item(i)->checkState() != Qt::Checked) continue;

			procedureCodesFilter.push_back(m_procedureCodes[i]);
		}
	}

	if (procedureCodesFilter.empty())
	{
		for (int i = 0; i < ui.nameFilterList->count(); i++)
		{
			procedureCodesFilter.push_back(m_procedureCodes[i]);
		}
	}

	return DentalStatistic{
		.year = ui.yearSpinBox->value(),

		.age = static_cast<DentalStatistic::AgeFilter>(ui.ageCombo->currentIndex()),

		.tooth = ui.ageCombo->isHidden() ?
					DentalStatistic::ToothFilter::All
					:
					static_cast<DentalStatistic::ToothFilter>(ui.toothCombo->currentIndex()),

		.financing = static_cast<DentalStatistic::FinancingFilter>(ui.financingCombo->currentIndex()),
		.nonTooth_specific = ui.toothCombo->isHidden(),
		.diagnosisFilter = diagnosisFilter,
		.procedureCodeFilter = procedureCodesFilter
	};

}

void StatisticDialogView::hideToothFilter(bool hidden)
{
	ui.toothCombo->setHidden(hidden);
	ui.label_4->setHidden(hidden);
}

void StatisticDialogView::setResult(int count)
{
	ui.resultLabel->setText("Oбщ брой: " + QString::number(count));
}
