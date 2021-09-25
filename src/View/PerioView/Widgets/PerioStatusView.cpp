#include "PerioStatusView.h"
#include "../PerioView/PerioChartItem.h"
#include "../PerioView/PerioScene.h"
#include "Measurment.h"
#include "Model/PerioStatus.h"

PerioStatusView::PerioStatusView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

}


/*
void PerioStatusView::disableColumn(int toothIdx, bool disabled)
{

	m_furcation[toothIdx]->setDisabled(disabled);
	m_mobi[toothIdx]->setDisabled(disabled);

	for (int i = 0; i < 3; i++)
	{
		int idxUp = toothIdx * 3 + i;
		int idxDown = 95 - idxUp;

		m_GM[idxUp]->disable(disabled);
		m_PD[idxUp]->disable(disabled);
		m_CAL[idxUp]->disable(disabled);
		m_BOP[idxUp]->setDisabled(disabled);

		m_GM[idxDown]->disable(disabled);
		m_PD[idxDown]->disable(disabled);
		m_CAL[idxDown]->disable(disabled);
		m_BOP[idxDown]->setDisabled(disabled);

		int GM = disabled ? 0 : m_GM[idxUp]->value();
		int CAL = disabled ? 0 : m_CAL[idxUp]->value();

		upperChart->setMeasurment(Measurment{ idxUp, m_GM[idxUp]->value(), m_CAL[idxUp]->value() });
		lowerChart->setMeasurment(Measurment{ 47-idxUp, m_GM[idxDown]->value(), m_CAL[idxDown]->value() });

	}
	m_Rec[toothIdx]->disable(disabled);
	m_Attach[toothIdx]->disable(disabled);


	m_Rec[31-toothIdx]->disable(disabled);
	m_Attach[31-toothIdx]->disable(disabled);

}

*/


PerioStatusView::~PerioStatusView()
{
}
