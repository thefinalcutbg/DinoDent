#include "PatientSummary.h"
#include <QPainter>
#include "Presenter/PatientSummaryPresenter/PatientSummaryPresenter.h"
#include "View/Theme.h"
PatientSummary::PatientSummary(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
	ui.teethView->setScene(&buccalScene);
	ui.teethView->setDisabled(true);

	ui.procedureTable->setModel(&m_procedureModel);
	ui.procedureTable->setProcedureHistoryLayout();
	ui.dateSlider->setTickPosition(QSlider::TickPosition::TicksBelow);

	connect(ui.dateSlider, &QSlider::valueChanged, this, [=] {presenter->setCurrentFrame(ui.dateSlider->value());});

	connect(ui.showLingual, &QCheckBox::stateChanged, this,
		[=] {
			if (ui.showLingual->isChecked()) {
				ui.teethView->setScene(&lingualScene);
			}
			else {
				ui.teethView->setScene(&buccalScene);
			}
		}
	);

	connect(ui.showPerio, &QCheckBox::stateChanged, this,
		[=] {
				bool show = ui.showPerio->isChecked();
				lingualScene.showPerio(show);
				buccalScene.showPerio(show);

		}
	);

	lingualScene.showPerio(false);
	buccalScene.showPerio(false);
}

PatientSummary::~PatientSummary()
{

}

void PatientSummary::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Theme::background);

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
	for (auto& t : teeth) {
		buccalScene.display(t);
		lingualScene.display(t);
	}

}

void PatientSummary::setProcedures(const std::vector<Procedure>& p)
{
	m_procedureModel.setProcedures(p);

	std::vector<int> treatedTeeth;
	
	for (auto& procedure : p)
		if(procedure.tooth != -1)
		treatedTeeth.push_back(procedure.tooth);

	
	buccalScene.setProcedures(treatedTeeth);
	lingualScene.setProcedures(treatedTeeth);

	this->setFixedHeight(710 + ui.procedureTable->height() + 100);

	update();
}

void PatientSummary::setPerioData(const PerioWithDisabled& perio)
{
	buccalScene.setMeasurments(perio.pd, perio.cal);
	lingualScene.setMeasurments(perio.pd, perio.cal);
}
