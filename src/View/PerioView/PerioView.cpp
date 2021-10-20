#include "PerioView.h"
#include "Model/PerioStatus.h"
#include "PerioView/PerioScene.h"
#include <QButtonGroup>
#include <QPainter>
#include "View/ToothPaintDevices/PaintHint.h"
#include "Widgets/Measurment.h"
#include "Presenter/PerioPresenter/PerioPresenter.h"
#include "Model/PerioToothData.h"

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

	initializeCommon();
	initializeSurfaces();
	initializeRecAndAtt();
	initializeTeethScenes();
	initializeFullMouth();
	
	for (int i = 0; i < 32; i++)
	{
		connect(m_tooth[i], &QPushButton::clicked, 
			[=] { presenter->toothButtonClicked(i); });
	}

	for (int i = 0; i < 192; i++)
	{
		connect(m_PD[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value) { presenter->pdChanged(i, value); });

		connect(m_CAL[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value) { presenter->calChanged(i, value); });

		connect(m_GM[i], QOverload<int>::of(&QSpinBox::valueChanged),
			[=](int value) { presenter->gmChanged(i, value); });

		connect(m_BOP[i], &QAbstractButton::clicked, [=] {presenter->bopChanged(i, m_BOP[i]->isChecked()); });
	}
}

void PerioView::PerioGraphicClicked(int index, PerioGraphicsType type)
{
	switch (type)
	{
	case PerioGraphicsType::Bleeding: 
		presenter->FMBSChanged(index, m_FMBS[index]->isChecked()); break;
	case PerioGraphicsType::Plaque:
		presenter->FMPSChanged(index, m_FMPS[index]->isChecked()); break;
	}
}

void PerioView::disableTooth(int index)
{
	auto uiTooth = getUIbyTooth(index);
	uiTooth.disable(true);

	for (int i = index * 3; i < index * 3 + 3; i++)
	{
		refreshChartMeasurment(i);
		refreshChartMeasurment(i + 96);
	}
}

void PerioView::setToothData(const PerioToothData& data)
{
	auto& idx = data.toothIndex;

	auto uiTooth = getUIbyTooth(idx);

	uiTooth.setData(data);

	for (int i = idx * 3; i < idx * 3 + 3; i++)
	{
		refreshChartMeasurment(i);
		refreshChartMeasurment(i + 96);
	}


}


void PerioView::setPresenter(PerioPresenter* presenter)
{
    this->presenter = presenter;

	if (!presenter)
	{
		perioScene[0]->deletePixmaps();
		perioScene[1]->deletePixmaps();
	}

}

void PerioView::setToothHint(const ToothPaintHint& hint)
{
	
	if (hint.idx < 16) perioScene[0]->display(hint);
	else perioScene[1]->display(hint);

	m_furcation[hint.idx]->setIndex(hint.idx);
	m_tooth[hint.idx]->setText(QString::number(hint.num));
}


void PerioView::setMeasurment(int index, int pd, int cal, int gm, int recession)
{
	QSignalBlocker b(m_PD[index]);
	QSignalBlocker c(m_CAL[index]);
	QSignalBlocker g(m_GM[index]);

	m_PD[index]->setValue(pd);
	m_CAL[index]->setValue(cal);
	m_GM[index]->setValue(gm);
	m_Rec[index / 3]->setValue(recession);

	refreshChartMeasurment(index);
}



void PerioView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::GlobalColor::white);
}


ToothUi PerioView::getUIbyTooth(int idx)
{
	ToothUi toothUi;

	toothUi.tooth = m_tooth[idx];
	toothUi.mobi = m_mobi[idx];
	toothUi.furcation = m_furcation[idx];
	toothUi.attachment[0] = m_Attach[idx];
	toothUi.attachment[1] = m_Attach[idx + 32];
	toothUi.recession[0] = m_Rec[idx];
	toothUi.recession[1] = m_Rec[idx + 32];

	for (int i = 0; i < 3; i++)
	{
		toothUi.pd[i] = m_PD[idx * 3 + i];
		toothUi.pd[i + 3] = m_PD[idx * 3 + i + lingualOffset];
		toothUi.cal[i] = m_CAL[idx * 3 + i];
		toothUi.cal[i + 3] = m_CAL[idx * 3 + i + lingualOffset];
		toothUi.gm[i] = m_GM[idx * 3 + i];
		toothUi.gm[i + 3] = m_GM[idx * 3 + i + lingualOffset];
		toothUi.bop[i] = m_BOP[idx * 3 + i];
		toothUi.bop[i + 3] = m_BOP[idx * 3 + i + lingualOffset];
	}

	for (int i = 0; i < 4; i++)
	{
		toothUi.FMBS[i] = m_FMBS[idx * 4 + i];
		toothUi.FMPS[i] = m_FMPS[idx * 4 + i];
	}

	return toothUi;

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





















//initializations

void PerioView::initializeCommon()
{
	SharedStates mobilityState(new std::vector<QString>{ "I", "II", "III" });

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
}

void PerioView::initializeSurfaces()
{
	static auto icon(QIcon(QPixmap("BOP.png")));

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

		m_PD[i + 96] = new PerioSpinBox(ui.maxilla);
		m_PD[i + 96]->setMaximum(19);
		ui.maxilla->ui.pdDownLayout->addWidget(m_PD[i + 96]);

		m_CAL[i + 96] = new PerioSpinBox(ui.maxilla);
		m_CAL[i + 96]->setMaximum(19);
		m_CAL[i + 96]->redValue = 4;
		ui.maxilla->ui.calDownLayout->addWidget(m_CAL[i + 96]);

		m_BOP[i + 96] = new PerioButton(ui.maxilla);
		m_BOP[i + 96]->setMinimumWidth(1);
		m_BOP[i + 96]->setCheckable(true);
		m_BOP[i + 96]->setIcon(icon);
		ui.maxilla->ui.bopDownLayout->addWidget(m_BOP[i + 96]);

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
		ui.mandibula->ui.pdDownLayout->addWidget(m_PD[i + 96]);

		m_CAL[i + 96] = new PerioSpinBox(ui.mandibula);
		m_CAL[i + 96]->setMaximum(19);
		m_CAL[i + 96]->redValue = 4;
		ui.mandibula->ui.calDownLayout->addWidget(m_CAL[i + 96]);

		m_BOP[i + 96] = new PerioButton(ui.mandibula);
		m_BOP[i + 96]->setMinimumWidth(1);
		m_BOP[i + 96]->setCheckable(true);
		m_BOP[i + 96]->setIcon(icon);
		ui.mandibula->ui.bopDownLayout->addWidget(m_BOP[i + 96]);

	}

	for (int i = 0; i < 48; i++)
	{
		m_GM[i] = new PerioSpinBox(ui.maxilla);
		m_GM[i]->setRange(-19, 19);
		ui.maxilla->ui.gmUpLayout->addWidget(m_GM[i]);

		m_GM[i + 96] = new PerioSpinBox(ui.maxilla);
		m_GM[i + 96]->setRange(-19, 19);
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
}

void PerioView::initializeFullMouth()
{
	QGraphicsScene* maxillaryScene = new QGraphicsScene;
	ui.maxilla->ui.graphicsSurfaceView->setScene(maxillaryScene);

	ui.maxilla->ui.graphicsSurfaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.maxilla->ui.graphicsSurfaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	int xPos = 0;

	for (int i = 0; i < 64; i++)
	{
		if (!(i % 4) && i > 3) xPos += surfButtonWidth;

		m_FMPS[i] = new PerioGraphicsButton(i, PerioGraphicsType::Plaque, this);
		maxillaryScene->addItem(m_FMPS[i]);
		m_FMPS[i]->setPos(xPos, 0);


		m_FMBS[i] = new PerioGraphicsButton(i, PerioGraphicsType::Bleeding, this);
		maxillaryScene->addItem(m_FMBS[i]);
		m_FMBS[i]->setPos(xPos, m_FMPS[i]->boundingRect().height());
	}

	QGraphicsScene* mandibularScene = new QGraphicsScene;
	ui.mandibula->ui.graphicsSurfaceView->setScene(mandibularScene);

	ui.mandibula->ui.graphicsSurfaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.mandibula->ui.graphicsSurfaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (int i = 64; i < 128; i++)
	{
		if (!(i % 4) && i > 67) xPos -= surfButtonWidth;

		m_FMPS[i] = new PerioGraphicsButton(i, PerioGraphicsType::Plaque, this);
		mandibularScene->addItem(m_FMPS[i]);
		m_FMPS[i]->setPos(xPos, 0);


		m_FMBS[i] = new PerioGraphicsButton(i, PerioGraphicsType::Bleeding, this);
		mandibularScene->addItem(m_FMBS[i]);
		m_FMBS[i]->setPos(xPos, m_FMPS[i]->boundingRect().height());
	}
}

void PerioView::initializeRecAndAtt()
{

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
		m_Attach[i + 32] = new PerioSpinBox(ui.maxilla);
		m_Attach[i + 32]->setMaximum(9);
		ui.maxilla->ui.agDownLayout->addWidget(m_Attach[i + 32]);

		m_Rec[i + 32] = new PerioSpinBox(ui.maxilla);
		m_Rec[i + 32]->setMaximum(19);
		m_Rec[i + 32]->setReadOnly(true);
		m_Rec[i + 32]->redValue = 1;
		ui.maxilla->ui.recDownLayout->addWidget(m_Rec[i + 32]);
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
		ui.mandibula->ui.agDownLayout->addWidget(m_Attach[i + 32]);

		m_Rec[i + 32] = new PerioSpinBox(ui.mandibula);
		m_Rec[i + 32]->setMaximum(19);
		m_Rec[i + 32]->setReadOnly(true);
		m_Rec[i + 32]->redValue = 1;
		ui.mandibula->ui.recDownLayout->addWidget(m_Rec[i + 32]);
	}

	//palatal surfaces have no attached gingiva!
	for (int i = 32; i < 48; i++)
		m_Attach[i]->disable(true);
}

void PerioView::initializeTeethScenes()
{
	//scene maxilaris:
	perioScene[0] = new PerioScene();

	upperChart[0] = new PerioChartItem();
	perioScene[0]->addItem(upperChart[0]);
	upperChart[0]->setPos(0, 0);

	lowerChart[0] = new PerioChartItem();
	perioScene[0]->addItem(lowerChart[0]);

	lowerChart[0]->setTransform(QTransform::fromScale(-1, 1));
	lowerChart[0]->setRotation(180);
	lowerChart[0]->setPos(0, 332);

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
	lowerChart[1]->setPos(lowerChart[1]->boundingRect().width(), 332);

	ui.mandibula->ui.graphicsView->setScene(perioScene[1]);

}