#include "PatientSummary.h"
#include <QPainter>
#include "Presenter/PatientSummaryPresenter/PatientSummaryPresenter.h"

PatientSummary::PatientSummary(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
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

void PatientSummary::setPatient(const Patient& patient)
{
}

void PatientSummary::setAmbDates(const std::vector<Date>& ambDates)
{
}

void PatientSummary::setTeeth(const ToothPaintHint& tooth)
{
}

void PatientSummary::setProcedures(const std::vector<Procedure>& m)
{
}
