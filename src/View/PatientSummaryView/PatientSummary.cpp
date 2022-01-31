#include "PatientSummary.h"
#include <QPainter>
#include "Presenter/PatientSummaryPresenter/PatientSummaryPresenter.h"

PatientSummary::PatientSummary(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
	ui.teethView->setScene(&m_teethScene);
	ui.teethView->setDisabled(true);

	ui.procedureTable->setModel(&m_procedureModel);
	ui.procedureTable->setProcedureHistoryLayout();
	ui.dateSlider->setTickPosition(QSlider::TickPosition::TicksBelow);

	connect(ui.dateSlider, &QSlider::valueChanged, this, [=] {presenter->setCurrentFrame(ui.dateSlider->value());});
}

PatientSummary::~PatientSummary()
{

}

void PatientSummary::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);

	painter.end();
}

void PatientSummary::setPresenter(PatientSummaryPresenter* presenter)
{
	this->presenter = presenter;
}

void PatientSummary::setDateLabel(const std::string& dateLabel)
{
	ui.dateLabel->setText(QString::fromStdString(dateLabel));
}

void PatientSummary::setTickPosition(int idx)
{
	ui.dateSlider->setValue(idx);
}

void PatientSummary::setTimeFrameCount(int count)
{
	ui.dateSlider->setRange(0, count - 1);
}

void PatientSummary::setPatient(const Patient& patient)
{
	ui.patientTile->setData(patient, Date::currentDate());
	ui.allergiesTile->setData(patient);
}


void PatientSummary::setTeeth(const std::array<ToothPaintHint, 32>& teeth)
{
	for (auto& t : teeth)
		m_teethScene.display(t);
}

void PatientSummary::setProcedures(const std::vector<Procedure>& p)
{
	m_procedureModel.setProcedures(p);

	std::vector<int> treatedTeeth;
	
	for (auto& procedure : p)
		if(procedure.tooth != -1)
		treatedTeeth.push_back(procedure.tooth);

	m_teethScene.setProcedures(treatedTeeth);

	//tova e copy-paste ot listView-to :
	ui.procedureTable->setHidden(!p.size());
	int tableHeight = p.size() * 50 + 26;
	//ne sym siguren izob6to, 4e taka iskam da izglejda:
	auto size = ui.procedureTable->size();
	size.setHeight(tableHeight);
	ui.procedureTable->setFixedSize(size);
	this->setFixedHeight(710 + tableHeight + 100);
}
