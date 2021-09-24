#include "PerioStatusView.h"
#include "../PerioView/PerioChartItem.h"
#include "../PerioView/PerioScene.h"
#include "Measurment.h"
#include "Model/PerioStatus.h"

PerioStatusView::PerioStatusView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    perioScene = new PerioScene();

    upperChart = new PerioChartItem();
    perioScene->addItem(upperChart);
    upperChart->setPos(0, 0);

    lowerChart = new PerioChartItem();
    perioScene->addItem(lowerChart);

    lowerChart->setTransform(QTransform::fromScale(-1, -1));
    lowerChart->setPos(lowerChart->boundingRect().width(), 360);

    ui.graphicsView->setScene(perioScene);

	//constexpr int toothidx[16]{ 18, 17,16,15,14,13,12,11,21,22,23,24,25,26,27,28 };

	static auto icon(QIcon(QPixmap("icons/BOP.png")));

	SharedStates mobilityState(new std::vector<QString>{ "I", "II", "III" });

	//initializing headers:

	for (int i = 0; i < 16; i++)
	{
		m_tooth[i] = new QPushButton(this);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setMinimumWidth(10);
	//	m_tooth[i]->setText(QString::number(toothidx[i]));
		ui.toothHeaderLayout->addWidget(m_tooth[i]);


	//	connect(m_tooth[i], &QPushButton::clicked,
	//		[=] { m_tooth[i]->setChecked(m_tooth[i]->isChecked()); });

		m_mobi[i] = new PerioStateButton(this);
		m_mobi[i]->setAvailableStates(mobilityState);
		m_mobi[i]->setMaximumHeight(20);
		ui.mobiLayout->addWidget(m_mobi[i]);


		m_furcation[i] = new FurcationWidget(this);
		ui.furcationLayout->addWidget(m_furcation[i]);
	}

	//upper data:

	for (int i = 0; i < 48; i++)
	{
		m_PD[i] = new PerioSpinBox(this);
		m_PD[i]->setMaximum(19);
		ui.pdUpLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(this);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.calUpLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(this);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(icon);
		ui.bopUpLayout->addWidget(m_BOP[i]);

	}

	for (int i = 0; i < 48; i++)
	{
		m_GM[i] = new PerioSpinBox(this);
		m_GM[i]->setRange(-19, 19);
		ui.gmUpLayout->addWidget(m_GM[i]);
	}

	for (int i = 0; i < 16; i++)
	{
		m_Attach[i] = new PerioSpinBox(this);
		m_Attach[i]->setMaximum(9);
		ui.agUpLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(this);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.recUpLayout->addWidget(m_Rec[i]);
	}

	//lower data:

	for (int i = 95; i >= 48; i--)
	{
		m_PD[i] = new PerioSpinBox(this);
		m_PD[i]->setMaximum(19);
		ui.pdDownLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(this);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.calDownLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(this);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(icon);
		ui.bopDownLayout->addWidget(m_BOP[i]);

	}

	for (int i = 95; i >= 48; i--)
	{
		m_GM[i] = new PerioSpinBox(this);
		m_GM[i]->setRange(-19, 19);
		ui.gmDownLayout->addWidget(m_GM[i]);
	}

	for (int i = 31; i >= 16; i--)
	{
		m_Attach[i] = new PerioSpinBox(this);
		m_Attach[i]->setMaximum(9);
		ui.agDownLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(this);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.recDownLayout->addWidget(m_Rec[i]);
	}


	for (int i = 0; i < 96; i++)
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

				if(i < 48) 
					upperChart->setMeasurment(Measurment{ i, m_GM[i]->value(), m_CAL[i]->value() });
				else
					lowerChart->setMeasurment(Measurment{ i-48, m_GM[i]->value(), m_CAL[i]->value() });
			});

	}

	for (int i = 0; i < 16; i++)
	{
		connect(m_tooth[i], &QPushButton::clicked, [=] { disableColumn(i, !m_tooth[i]->isChecked()); });
	}

}


void PerioStatusView::calcRecession(int GM_idx)
{
	int r_idx = GM_idx / 3;

	int recession = 0;
	for (int i = r_idx * 3; i < r_idx * 3 + 3; i++)
	{
		int recTemp = m_GM[i]->value() * (-1);
		recession = std::max(recession, recTemp);
	}

	m_Rec[r_idx]->setValue(recession);
}

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


void PerioStatusView::setTooth(const ToothPaintHint& hint)
{
    perioScene->display(hint);

	int index = hint.idx < 16 ? hint.idx : 31 - hint.idx;

	m_furcation[index]->setIndex(hint.idx);
	m_tooth[index]->setText(QString::number(hint.num));
}

void PerioStatusView::setPerioStatus(const PerioStatus& status)
{
	for (int i = 0; i < perioSurfCount; i++)
	{
		m_CAL[i]->setValue(status.cal[i]);
		m_PD[i]->setValue(status.pd[i]);
		m_BOP[i]->setChecked(status.bop[i]);
	}

	for (int i = 0; i < 32; i++)
	{
		m_Attach[i]->setValue(status.ag[i]);
	}

	for (int i = 0; i < 16; i++)
	{
		if (m_tooth[i]->isChecked() != !status.disabled[i])
			m_tooth[i]->click();

		m_mobi[i]->setState(status.mobility[i]);
	}

}

PerioStatus PerioStatusView::getPerioStatus()
{
	PerioStatus status;

	for (int i = 0; i < perioSurfCount; i++)
	{
		status.cal[i] = m_CAL[i]->value();
		status.pd[i] = m_PD[i]->value();
		status.bop[i] = m_BOP[i]->isChecked();
	}

	for (int i = 0; i < 32; i++)
	{
		status.ag[i] = m_Attach[i]->value();
	}

	for (int i = 0; i < 16; i++)
	{
		status.disabled[i] = !m_tooth[i]->isChecked();
		status.mobility[i] = m_mobi[i]->getState();
	}

	return status;
}



PerioStatusView::~PerioStatusView()
{
}
