#include "PerioSurfaceData.h"
#include <algorithm>
#include <QIcon>
#include <QDebug>
PerioSurfaceData::PerioSurfaceData(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setMinimumHeight(101);

	static auto icon(QIcon(QPixmap("icons/BOP.png")));

	for (int i = 0; i < 48; i++)
	{
		m_PD[i] = new PerioSpinBox(this);
		m_PD[i]->setMaximum(19);
		ui.pdLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(this);
		m_CAL[i]->setMaximum(15);
		m_CAL[i]->redValue = 4;
		ui.calLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(this);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(icon);
		ui.bopLayout->addWidget(m_BOP[i]);

	}

	for (int i = 0; i < 48; i++)
	{
		m_GM[i] = new PerioSpinBox(this);
		m_GM[i]->setRange(-15, 19);
		ui.gmLayout->addWidget(m_GM[i]);
	}

	for (int i = 0; i < 16; i++)
	{
		m_Attach[i] = new PerioSpinBox(this);
		m_Attach[i]->setMaximum(9);
		ui.attGingLayout->addWidget(m_Attach[i]);
		
		m_Rec[i] = new PerioSpinBox(this);
		m_Rec[i]->setMaximum(15);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.recesLayout->addWidget(m_Rec[i]);
	}

	for (int i = 0; i < 48; i++)
	{
		connect(m_PD[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value)
			{ 
				m_GM[i]->setValue(value - m_CAL[i]->value()); 
			});

		connect(m_CAL[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value)
			{ 
				m_GM[i]->setValue(m_PD[i]->value() - m_CAL[i]->value()); 
			});

		connect(m_GM[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value)
			{ 
				m_CAL[i]->setValue(m_PD[i]->value() - value);
				calcRecession(i);
				emit measuremetChanged(Measurment{ i, m_GM[i]->value(), m_CAL[i]->value() });
			});

	}

}

void PerioSurfaceData::setExtracted(int tooth, bool extracted)
{

	for (int i = 0; i < 3; i++)
	{
		int idx = tooth * 3 + i;
		m_GM[idx]->disable(extracted);
		m_PD[idx]->disable(extracted);
		m_CAL[idx]->disable(extracted);
		m_BOP[idx]->setDisabled(extracted);
		
		int GM = extracted ? 0 : m_GM[idx]->value();
		int CAL = extracted ? 0 : m_CAL[idx]->value();

		emit measuremetChanged(Measurment{ idx, GM, CAL });

	}
	m_Rec[tooth]->disable(extracted);
	m_Attach[tooth]->disable(extracted);


}

void PerioSurfaceData::setLingual(bool lingual)
{
	lingual ?
		ui.verticalLayout->setDirection(QBoxLayout::Direction::BottomToTop)
		:
		ui.verticalLayout->setDirection(QBoxLayout::Direction::TopToBottom);
}


void PerioSurfaceData::calcRecession(int GM_idx)
{
	int r_idx = GM_idx/3;
	
	int recession = 0;
	for ( int i = r_idx*3; i < r_idx*3 + 3; i++)
	{
		int recTemp = m_GM[i]->value()*(-1);
		recession = std::max(recession, recTemp);
	}

	m_Rec[r_idx]->setValue(recession);
}


PerioSurfaceData::~PerioSurfaceData()
{
}

