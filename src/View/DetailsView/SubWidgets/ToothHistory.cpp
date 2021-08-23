#include "ToothHistory.h"

ToothHistory::ToothHistory(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void ToothHistory::setModel(const std::vector<DetailsSummary>& summary)
{
	m_model.setProcedure(summary);
	ui.procedureTable->setModel(&m_model);
}

ToothHistory::~ToothHistory()
{
}
