#include "StatisticDialogView.h"
#include "Presenter/StatisticDialogPresenter.h"
#include "Model/Dental/Diagnosis.h"
#include <QPainter>

void StatisticDialogView::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);
}

std::array<std::pair<QString, ProcedureType>, (int)ProcedureType::MaxCount>s_idxMapping = {

	std::make_pair(QString("Възстановяване"), ProcedureType::Restoration),
	std::make_pair(QString("Щифтово изграждане"), ProcedureType::PostCore),
	std::make_pair(QString("Ендодонтска процедура"), ProcedureType::Endodontic),
	std::make_pair(QString("Поставяне на радикуларен щифт"), ProcedureType::Post),

	std::make_pair(QString("Екстракция"), ProcedureType::Extraction),
	std::make_pair(QString("Множествена екстракция"), ProcedureType::MultipleExtraction),

	std::make_pair(QString("Имплант"), ProcedureType::Implant),
	
	std::make_pair(QString("Корона/Блок корони"), ProcedureType::Crown),
	std::make_pair(QString("Ендокорона"), ProcedureType::PostCrown),
	std::make_pair(QString("Корона, Мостова конструкция или фасета"), ProcedureType::CrownOrBridgeOrVeneer),
	std::make_pair(QString("Мостова конструкция"), ProcedureType::Bridge),
	std::make_pair(QString("Шиниране/Адхезивен мост"), ProcedureType::Splint),

	std::make_pair(QString("Протеза"), ProcedureType::Denture),
	std::make_pair(QString("Комплект протези"), ProcedureType::DenturePair),

	std::make_pair(QString("Премахване на възстановяване"), ProcedureType::RemoveRestoration),
	std::make_pair(QString("Премахване на радикуларен щифт"), ProcedureType::RemovePost),
	std::make_pair(QString("Премахване на фиксирана протетична конструкция"), ProcedureType::RemoveCrownOrBridge),

	std::make_pair(QString("Почистване на зъбен камък"), ProcedureType::Depuratio),
	std::make_pair(QString("Почистване на зъбен камък (на квадрант)"), ProcedureType::DepuratioQuadrant),
	std::make_pair(QString("Почистване на зъбен камък (на зъб)"), ProcedureType::DepuratioTooth),

	std::make_pair(QString("Обстоен преглед и сваляне на зъбен статус"), ProcedureType::FullExam),

	std::make_pair(QString("Други процедури"), ProcedureType::General),
	std::make_pair(QString("Други процедури засягащи зъб/зъби"), ProcedureType::ToothNonSpecific),
	std::make_pair(QString("Обща анестезия"), ProcedureType::Anesthesia)
};

StatisticDialogView::StatisticDialogView(StatisticDialogPresenter& p, QWidget *parent)
    : QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	setWindowTitle("Статистики");
	setWindowIcon(QIcon(":/icons/icon_statistic.png"));
	setWindowFlag(Qt::WindowMaximizeButtonHint);

	for (auto& [descr, type] : s_idxMapping) {
		ui.procedureCombo->addItem(descr);
	}

	connect(ui.procedureCombo, &QComboBox::currentIndexChanged, [&](int idx) {presenter.setProcedureType(static_cast<int>(s_idxMapping[idx].second));});

	for (auto &icd : ICD10::getDentalICDCodes())
	{
		QListWidgetItem* item = new QListWidgetItem(icd.name().c_str(), ui.diagnosisFilterList);
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		item->setCheckState(Qt::Unchecked);
		ui.diagnosisFilterList->addItem(item);
	}

	ui.yearSpinBox->setValue(Date::currentDate().year);

	presenter.setView(this);

	ui.procedureCombo->setCurrentIndex(0);

    connect(ui.yearSpinBox, &QSpinBox::valueChanged, this, [&] { presenter.parameterChanged(); });
    connect(ui.ageCombo, &QComboBox::currentIndexChanged, this, [&]{ presenter.parameterChanged(); });
    connect(ui.toothCombo, &QComboBox::currentIndexChanged, this, [&] { presenter.parameterChanged(); });
    connect(ui.financingCombo, &QComboBox::currentIndexChanged, this, [&] { presenter.parameterChanged(); });
    connect(ui.diagnosisGroup, &QGroupBox::clicked, this, [&] { presenter.parameterChanged(); });
    connect(ui.nameGroup,&QGroupBox::clicked, this, [&] { presenter.parameterChanged(); });
    connect(ui.nameFilterList, &QListWidget::itemChanged, this, [&] { presenter.parameterChanged(); });
    connect(ui.diagnosisFilterList, &QListWidget::itemChanged, this, [&] { presenter.parameterChanged(); });

	emit ui.procedureCombo->currentIndexChanged(0);
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

DentalStatistic StatisticDialogView::getStatistic()
{

	decltype(DentalStatistic::diagnosisFilter) diagnosisFilter;
	
	if (ui.diagnosisGroup->isChecked())
	{
		for (int i = 0; i < ui.diagnosisFilterList->count(); i++) {
			
			if (ui.diagnosisFilterList->item(i)->checkState() != Qt::Checked) continue;


			diagnosisFilter.push_back(ICD10::getCodeFromName(ui.diagnosisFilterList->item(i)->text().toStdString()));
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
