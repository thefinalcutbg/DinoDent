#include "PerioView.h"
#include "Model/PerioStatus.h"
#include "PerioView/PerioScene.h"
#include <QButtonGroup>
#include <QPainter>
#include <QDebug>
#include "View/ToothPaintDevices/PaintHint.h"
#include "Widgets/Measurment.h"
#include <QDebug>
PerioView::PerioView(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QButtonGroup* group = new QButtonGroup(this);
	group->addButton(ui.upperButton);
	group->addButton(ui.lowerButton);
	group->setExclusive(true);

	ui.upperButton->setChecked(true);
	connect(ui.upperButton, &QPushButton::clicked, [&] { ui.stackedWidget->setCurrentWidget(ui.perioUpper); });
	connect(ui.lowerButton, &QPushButton::clicked, [&] { ui.stackedWidget->setCurrentWidget(ui.perioLower); });

	addToLayout(
		ui.perioUpper,
		perioScene[0],
		upperChart[0],
		lowerChart[0],
		&m_tooth[0],
		&m_mobi[0],
		&m_furcation[0],
		&m_Rec[0],
		&m_Attach[0],
		&m_PD[0],
		&m_CAL[0],
		&m_GM[0],
		&m_BOP[0]);

	addToLayout(
		ui.perioLower,
		perioScene[1],
		upperChart[1],
		lowerChart[1],
		&m_tooth[16],
		&m_mobi[16],
		&m_furcation[16],
		&m_Rec[32],
		&m_Attach[32],
		&m_PD[96],
		&m_CAL[96],
		&m_GM[96],
		&m_BOP[96]);

	for (int i = 16; i < 32; i++) m_Attach[i]->disable(true);

	for (int i = 0; i < 32; i++)
	{
		connect(m_tooth[i], &QPushButton::clicked, [=] { disableColumn(i, !m_tooth[i]->isChecked()); });
	}

	for (int i = 0; i < 192; i++)
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
	}
    
}

void PerioView::setPresenter(PerioPresenter* presenter)
{
    this->presenter = presenter;
}

void PerioView::setToothHint(const ToothPaintHint& hint)
{
	
	if (hint.idx < 16) perioScene[0]->display(hint);
	else perioScene[1]->display(hint);

	m_furcation[hint.idx]->setIndex(hint.idx);
	m_tooth[hint.idx]->setText(QString::number(hint.num));
}

void PerioView::setPerioStatus(const PerioStatus& status)
{
	for (int i = 0; i < perioSurfCount; i++)
	{
		m_CAL[i]->setValue(status.cal[i]);
		m_PD[i]->setValue(status.pd[i]);
		m_BOP[i]->setChecked(status.bop[i]);
	}

	for (int i = 0; i < 64; i++)
	{
		m_Attach[i]->setValue(status.ag[i]);
	}

	for (int i = 0; i < 32; i++)
	{
		if (m_tooth[i]->isChecked() != !status.disabled[i])
			m_tooth[i]->click();

		m_mobi[i]->setState(status.mobility[i]);
	}


}

PerioStatus PerioView::getPerioStatus()
{
	PerioStatus status;

	for (int i = 0; i < perioSurfCount; i++)
	{
		status.cal[i] = m_CAL[i]->value();
		status.pd[i] = m_PD[i]->value();
		status.bop[i] = m_BOP[i]->isChecked();
	}

	for (int i = 0; i < 64; i++)
	{
		status.ag[i] = m_Attach[i]->value();
	}

	for (int i = 0; i < 32; i++)
	{
		status.disabled[i] = !m_tooth[i]->isChecked();
		status.mobility[i] = m_mobi[i]->getState();
	}

	return status;
}


void PerioView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::GlobalColor::white);
}

void PerioView::calcRecession(int GM_idx)
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

void PerioView::disableColumn(int toothIdx, bool disabled)
{
	int& i = toothIdx;
	m_tooth[i]->setChecked(!disabled);

	m_mobi[i]->setDisabled(disabled);
	m_furcation[i]->setDisabled(disabled);

	int recPos = toothIdx < 16 ? i : 16 + i;

	m_Attach[recPos]->disable(disabled);
	if(i > 15) m_Attach[recPos +16]->disable(disabled);
	m_Rec[recPos]->disable(disabled);
	m_Rec[recPos + 16]->disable(disabled);

	int surfPosition = i < 16 ? i * 3 : 96 + ((i-16) * 3);
	int endPosition = surfPosition + 3;

	for (int y = surfPosition; y < endPosition; y++)
	{
		m_BOP[y]->setDisabled(disabled);
		m_PD[y]->disable(disabled);
		m_CAL[y]->disable(disabled);
		m_GM[y]->disable(disabled);
		
		
		m_BOP[y + 48]->setDisabled(disabled);
		m_PD[y + 48]->disable(disabled);
		m_CAL[y + 48]->disable(disabled);
		m_GM[y + 48]->disable(disabled);

		if (i < 16)
		{
			upperChart[0]->setMeasurment(Measurment{ y, m_GM[y]->value(), m_CAL[y]->value() });
			lowerChart[0]->setMeasurment(Measurment{ y, m_GM[y + 48]->value(), m_CAL[y + 48]->value() });
		}
		else
		{
			upperChart[1]->setMeasurment(Measurment{ y-96, m_GM[y]->value(), m_CAL[y]->value() });
			lowerChart[1]->setMeasurment(Measurment{ y-96, m_GM[y + 48]->value(), m_CAL[y + 48]->value() });
		}
		
	}


}

void PerioView::addToLayout(PerioStatusView* view, 
							PerioScene* &perioScene,
							PerioChartItem* &upperChart,
							PerioChartItem* &lowerChart, 
							QPushButton* m_tooth[16], 
							PerioStateButton* m_mobi[16], 
							FurcationWidget* m_furcation[16],
							PerioSpinBox* m_Rec[32], 
							PerioSpinBox* m_Attach[32], 
							PerioSpinBox* m_PD[96], 
							PerioSpinBox* m_CAL[96], 
							PerioSpinBox* m_GM[96], 
							PerioButton* m_BOP[96])
{
	static auto icon(QIcon(QPixmap("icons/BOP.png")));
	SharedStates mobilityState(new std::vector<QString>{ "I", "II", "III" });

	auto& ui = view->ui;

	perioScene = new PerioScene();

	upperChart = new PerioChartItem();
	perioScene->addItem(upperChart);
	upperChart->setPos(0, 0);

	lowerChart = new PerioChartItem();
	perioScene->addItem(lowerChart);

	lowerChart->setTransform(QTransform::fromScale(-1, 1));
	lowerChart->setRotation(180);
	lowerChart->setPos(0, 360);

	ui.graphicsView->setScene(perioScene);

	for (int i = 0; i < 16; i++)
	{
		m_tooth[i] = new QPushButton(view);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setMinimumWidth(10);

		ui.toothHeaderLayout->addWidget(m_tooth[i]);


		m_mobi[i] = new PerioStateButton(view);
		m_mobi[i]->setAvailableStates(mobilityState);
		m_mobi[i]->setMaximumHeight(20);
		ui.mobiLayout->addWidget(m_mobi[i]);


		m_furcation[i] = new FurcationWidget(view);
		ui.furcationLayout->addWidget(m_furcation[i]);
	}

	//upper data:

	for (int i = 0; i < 48; i++)
	{
		m_PD[i] = new PerioSpinBox(view);
		m_PD[i]->setMaximum(19);
		ui.pdUpLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(view);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.calUpLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(view);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(view);
		m_BOP[i]->setIcon(icon);
		ui.bopUpLayout->addWidget(m_BOP[i]);

	}

	for (int i = 0; i < 48; i++)
	{
		m_GM[i] = new PerioSpinBox(view);
		m_GM[i]->setRange(-19, 19);
		ui.gmUpLayout->addWidget(m_GM[i]);

		connect(m_GM[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value)
			{
				m_CAL[i]->setValue(m_PD[i]->value() - value);

				calcRecession(i);

				upperChart->setMeasurment(Measurment{ i, m_GM[i]->value(), m_CAL[i]->value() });
				
			});
	}

	for (int i = 0; i < 16; i++)
	{
		m_Attach[i] = new PerioSpinBox(view);
		m_Attach[i]->setMaximum(9);
		ui.agUpLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(view);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.recUpLayout->addWidget(m_Rec[i]);
	}

	//lower data:

	for (int i = 48; i < 96; i++)
	{
		m_PD[i] = new PerioSpinBox(view);
		m_PD[i]->setMaximum(19);
		ui.pdDownLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(view);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.calDownLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(view);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(icon);
		ui.bopDownLayout->addWidget(m_BOP[i]);

	}

	for (int i = 48; i < 96; i++)
	{
		m_GM[i] = new PerioSpinBox(view);
		m_GM[i]->setRange(-19, 19);
		ui.gmDownLayout->addWidget(m_GM[i]);

		connect(m_GM[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value)
			{
				m_CAL[i]->setValue(m_PD[i]->value() - value);

				calcRecession(i);

				lowerChart->setMeasurment(Measurment{ i - 48, m_GM[i]->value(), m_CAL[i]->value() });
			});
	}

	for (int i = 16; i < 32; i++)
	{
		m_Attach[i] = new PerioSpinBox(view);
		m_Attach[i]->setMaximum(9);
		ui.agDownLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(view);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(view);
		m_Rec[i]->redValue = 1;
		ui.recDownLayout->addWidget(m_Rec[i]);
	}

}
