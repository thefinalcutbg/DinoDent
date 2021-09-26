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
	connect(ui.upperButton, &QPushButton::clicked, [&] { ui.stackedWidget->setCurrentWidget(ui.maxilla); });
	connect(ui.lowerButton, &QPushButton::clicked, [&] { ui.stackedWidget->setCurrentWidget(ui.mandibula); });

	static auto icon(QIcon(QPixmap("icons/BOP.png")));
	SharedStates mobilityState(new std::vector<QString>{ "I", "II", "III" });


	//BRUTE FORCE INITIALIZATION MOTHERFUCKER
	
	//initializing common stuff for maxillary view
	for (int i = 0; i < 16; i++)
	{
		m_tooth[i] = new QPushButton(ui.maxilla);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setMinimumWidth(10);
		
		ui.maxilla->ui.toothHeaderLayout->addWidget(m_tooth[i]);

		m_mobi[i] = new PerioStateButton(ui.maxilla);
		m_mobi[i]->setAvailableStates(mobilityState);
		m_mobi[i]->setMaximumHeight(20);
		ui.maxilla->ui.mobiLayout->addWidget(m_mobi[i]);


		m_furcation[i] = new FurcationWidget(ui.maxilla);
		ui.maxilla->ui.furcationLayout->addWidget(m_furcation[i]);
	}

	//initializing common stuff for mandibular view
	for (int i = 31; i >= 16; i--)
	{
		m_tooth[i] = new QPushButton(ui.mandibula);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setMinimumWidth(10);

		ui.mandibula->ui.toothHeaderLayout->addWidget(m_tooth[i]);


		m_mobi[i] = new PerioStateButton(ui.mandibula);
		m_mobi[i]->setAvailableStates(mobilityState);
		m_mobi[i]->setMaximumHeight(20);
		ui.mandibula->ui.mobiLayout->addWidget(m_mobi[i]);


		m_furcation[i] = new FurcationWidget(ui.mandibula);
		ui.mandibula->ui.furcationLayout->addWidget(m_furcation[i]);
	}

	//initializing PD, CAL, GM for maxillary view:

	for (int i = 0; i < 48; i++)
	{
		//the buccal row:
		m_PD[i] = new PerioSpinBox(ui.maxilla);
		m_PD[i]->setMaximum(19);
		ui.maxilla->ui.pdUpLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(ui.maxilla);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.maxilla->ui.calUpLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(ui.maxilla);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(icon);
		ui.maxilla->ui.bopUpLayout->addWidget(m_BOP[i]);

		//the lingual row:

		m_PD[i+96] = new PerioSpinBox(ui.maxilla);
		m_PD[i + 96]->setMaximum(19);
		ui.maxilla->ui.pdDownLayout->addWidget(m_PD[i + 96]);

		m_CAL[i + 96] = new PerioSpinBox(ui.maxilla);
		m_CAL[i + 96]->setMaximum(19);
		m_CAL[i + 96]->redValue = 4;
		ui.maxilla->ui.calDownLayout->addWidget(m_CAL[i+96]);

		m_BOP[i + 96] = new PerioButton(ui.maxilla);
		m_BOP[i + 96]->setMinimumWidth(1);
		m_BOP[i + 96]->setCheckable(true);
		m_BOP[i + 96]->setIcon(icon);
		ui.maxilla->ui.bopDownLayout->addWidget(m_BOP[i+96]);

	}

	//now the mandibular view :@ :@ :@ :@

	for (int i = 95; i >= 48; i--)
	{
		//the buccal row:
		m_PD[i] = new PerioSpinBox(ui.mandibula);
		m_PD[i]->setMaximum(19);
		ui.mandibula->ui.pdUpLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(ui.maxilla);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.mandibula->ui.calUpLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(ui.mandibula);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(icon);
		ui.mandibula->ui.bopUpLayout->addWidget(m_BOP[i]);

		//the lingual row:

		m_PD[i + 96] = new PerioSpinBox(ui.mandibula);
		m_PD[i + 96]->setMaximum(19);
		ui.mandibula->ui.pdDownLayout->addWidget(m_PD[i+96]);

		m_CAL[i + 96] = new PerioSpinBox(ui.mandibula);
		m_CAL[i + 96]->setMaximum(19);
		m_CAL[i + 96]->redValue = 4;
		ui.mandibula->ui.calDownLayout->addWidget(m_CAL[i+96]);

		m_BOP[i + 96] = new PerioButton(ui.mandibula);
		m_BOP[i + 96]->setMinimumWidth(1);
		m_BOP[i + 96]->setCheckable(true);
		m_BOP[i + 96]->setIcon(icon);
		ui.mandibula->ui.bopDownLayout->addWidget(m_BOP[i+96]);

	}

	//now for the fuckin gingival margin spinbox, which needs its own tab focus order :@ :@ :@

	for (int i = 0; i < 48; i++)
	{
		m_GM[i] = new PerioSpinBox(ui.maxilla);
		m_GM[i]->setRange(-19, 19);
		ui.maxilla->ui.gmUpLayout->addWidget(m_GM[i]);
		
		m_GM[i+96] = new PerioSpinBox(ui.maxilla);
		m_GM[i+96]->setRange(-19, 19);
		ui.maxilla->ui.gmDownLayout->addWidget(m_GM[i + 96]);

	}

	//the mandibular view:
	for (int i = 95; i >= 48; i--)
	{
		//buccal:
		m_GM[i] = new PerioSpinBox(ui.mandibula);
		m_GM[i]->setRange(-19, 19);
		ui.mandibula->ui.gmUpLayout->addWidget(m_GM[i]);

		//lingual:
		m_GM[i + 96] = new PerioSpinBox(ui.mandibula);
		m_GM[i + 96]->setRange(-19, 19);
		ui.mandibula->ui.gmDownLayout->addWidget(m_GM[i + 96]);
	}

	//recession and attachment spinboxes for maxilla:

	for (int i = 0; i < 16; i++)
	{
		//buccal:
		m_Attach[i] = new PerioSpinBox(ui.maxilla);
		m_Attach[i]->setMaximum(9);
		ui.maxilla->ui.agUpLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(ui.maxilla);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.maxilla->ui.recUpLayout->addWidget(m_Rec[i]);

		//lingual:
		m_Attach[i+32] = new PerioSpinBox(ui.maxilla);
		m_Attach[i + 32]->setMaximum(9);
		ui.maxilla->ui.agDownLayout->addWidget(m_Attach[i+32]);

		m_Rec[i + 32] = new PerioSpinBox(ui.maxilla);
		m_Rec[i + 32]->setMaximum(19);
		m_Rec[i + 32]->setReadOnly(true);
		m_Rec[i + 32]->redValue = 1;
		ui.maxilla->ui.recDownLayout->addWidget(m_Rec[i+32]);
	}

	//recession and attachment sponboxes for mandibula:

	for (int i = 31; i >= 16; i--)
	{
		//buccal:
		m_Attach[i] = new PerioSpinBox(ui.mandibula);
		m_Attach[i]->setMaximum(9);
		ui.mandibula->ui.agUpLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(ui.mandibula);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.mandibula->ui.recUpLayout->addWidget(m_Rec[i]);

		//lingual:
		m_Attach[i + 32] = new PerioSpinBox(ui.mandibula);
		m_Attach[i + 32]->setMaximum(9);
		ui.mandibula->ui.agDownLayout->addWidget(m_Attach[i+32]);

		m_Rec[i + 32] = new PerioSpinBox(ui.mandibula);
		m_Rec[i + 32]->setMaximum(19);
		m_Rec[i + 32]->setReadOnly(true);
		m_Rec[i + 32]->redValue = 1;
		ui.mandibula->ui.recDownLayout->addWidget(m_Rec[i+32]);
	}

	//initializing tooth scenes:
	//scene maxilaris:
	perioScene[0] = new PerioScene();

	upperChart[0] = new PerioChartItem();
	perioScene[0]->addItem(upperChart[0]);
	upperChart[0]->setPos(0, 0);

	lowerChart[0] = new PerioChartItem();
	perioScene[0]->addItem(lowerChart[0]);

	lowerChart[0]->setTransform(QTransform::fromScale(-1, 1));
	lowerChart[0]->setRotation(180);
	lowerChart[0]->setPos(0, 360);

	ui.maxilla->ui.graphicsView->setScene(perioScene[0]);

	//scene mandibularis:
	perioScene[1] = new PerioScene();

	upperChart[1] = new PerioChartItem();
	perioScene[1]->addItem(upperChart[1]);
	upperChart[1]->setRotation(180);
	upperChart[1]->setTransform(QTransform::fromScale(1, -1));
	upperChart[1]->setPos(upperChart[1]->boundingRect().width(), 0);

	lowerChart[1] = new PerioChartItem();
	perioScene[1]->addItem(lowerChart[1]);

	lowerChart[1]->setTransform(QTransform::fromScale(-1, -1));
	lowerChart[1]->setPos(lowerChart[1]->boundingRect().width(), 360);

	ui.mandibula->ui.graphicsView->setScene(perioScene[1]);

	//initializing FMBS/FMPS:
	QGraphicsScene* maxillaryScene = new QGraphicsScene;
	ui.maxilla->ui.graphicsSurfaceView->setScene(maxillaryScene);

	ui.maxilla->ui.graphicsSurfaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.maxilla->ui.graphicsSurfaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	int xPos = 0;

	for (int i = 0; i < 16; i++)
	{
		
		for (int y = 0; y < 4; y++)
		{
			int index = i*4 + y;

			m_FMPS[index] = new PerioGraphicsButton(static_cast<ButtonPos>(y), this);
			maxillaryScene->addItem(m_FMPS[index]);
			m_FMPS[index]->setPos(xPos, 0);
			

			m_FMBS[index] = new PerioGraphicsButton(static_cast<ButtonPos>(y), this);
			maxillaryScene->addItem(m_FMBS[index]);
			m_FMBS[index]->setPos(xPos, m_FMBS[index]->boundingRect().height());
			m_FMBS[index]->setColor(QColor{ 255,146,148 });
		}

		xPos = xPos + surfButtonWidth;

	}

	QGraphicsScene* mandibularScene = new QGraphicsScene;
	ui.mandibula->ui.graphicsSurfaceView->setScene(mandibularScene);

	ui.mandibula->ui.graphicsSurfaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.mandibula->ui.graphicsSurfaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (int i = 31; i >= 16; i--)
	{

		for (int y = 0; y < 4; y++)
		{
			int index = i * 4 + y;

			m_FMPS[index] = new PerioGraphicsButton(static_cast<ButtonPos>(y), this);
			mandibularScene->addItem(m_FMPS[index]);
			m_FMPS[index]->setPos(xPos, 0);


			m_FMBS[index] = new PerioGraphicsButton(static_cast<ButtonPos>(y), this);
			mandibularScene->addItem(m_FMBS[index]);
			m_FMBS[index]->setPos(xPos, m_FMBS[index]->boundingRect().height());
			m_FMBS[index]->setColor(QColor{ 255,146,148 });
		}

		xPos = xPos + surfButtonWidth;

	}




	//palatal surfaces have no attached gingiva!
	for (int i = 32; i < 48; i++) 
		m_Attach[i]->disable(true);
	
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

		connect(m_GM[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value)
			{
				m_CAL[i]->setValue(m_PD[i]->value() - value);

				calculateCalAvg();
				calcRecession(i);
				refreshChartMeasurment(i);

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
	
	m_tooth[toothIdx]->setChecked(!disabled);

	m_mobi[toothIdx]->setDisabled(disabled);
	m_furcation[toothIdx]->setDisabled(disabled);

	for (int i = toothIdx*4; i < (toothIdx*4)+4; i++)
	{
		m_FMBS[i]->setDisabled(disabled);
		m_FMPS[i]->setDisabled(disabled);
	}

	
	m_Attach[toothIdx]->disable(disabled);
	if (!(toothIdx + 32 < 46))
	m_Attach[toothIdx +32]->disable(disabled);
	m_Rec[toothIdx]->disable(disabled);
	m_Rec[toothIdx +32]->disable(disabled);

	for (int i = toothIdx*3; i < toothIdx*3 + 3; i++)
	{
		m_BOP[i]->setDisabled(disabled);
		m_PD[i]->disable(disabled);
		m_CAL[i]->disable(disabled);
		m_GM[i]->disable(disabled);
		
		refreshChartMeasurment(i);
		
		m_BOP[i + 96]->setDisabled(disabled);
		m_PD[i + 96]->disable(disabled);
		m_CAL[i + 96]->disable(disabled);
		m_GM[i + 96]->disable(disabled);

		refreshChartMeasurment(i + 96);
		
	}

	ui.calAvg->setText("CAL: " + QString::number(calculateCalAvg(), 'f', 2));

}

double PerioView::calculateCalAvg()
{
	int sum{ 0 };

	int count{ 0 };

	for (int i = 0; i < 192; i++)
	{
		if (!m_CAL[i]->isEnabled()) continue;

		sum = sum + m_CAL[i]->value();
		count++;
	}

	return sum /(double)count;
}

void PerioView::refreshChartMeasurment(int idx)
{
	int GM = m_GM[idx]->value();
	int CAL = m_CAL[idx]->value();
	int position = 0;

	if (idx < 48)
	{
		int position = idx;
		upperChart[0]->setMeasurment(Measurment{ position, GM, CAL });
	}
	else if (idx < 96)
	{
		int position = idx - 48;
		upperChart[1]->setMeasurment(Measurment{ position, GM, CAL });
	}
	else if (idx < 144)
	{
		int position = idx - 96;
		lowerChart[0]->setMeasurment(Measurment{ position, GM, CAL });
	}
	else
	{
		int position = idx - 144;
		lowerChart[1]->setMeasurment(Measurment{ position, GM, CAL });
	}
}
