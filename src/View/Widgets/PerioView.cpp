#include "PerioView.h"

#include <QButtonGroup>
#include <QPainter>

#include "View/Graphics/PaintHint.h"
#include "Presenter/PerioPresenter.h"
#include "Model/Dental/PerioToothData.h"
#include "Model/Dental/PerioStatistic.h"
#include "Model/Dental/PerioStatistic.h"
#include "View/Graphics/PerioScene.h"
#include "Model/Date.h"

#include "View/Theme.h"
#include "View/Graphics/PerioChartItem.h"
#include "View/Graphics/PerioGraphicsButton.h"
#include "View/Graphics/PerioScene.h"
#include "View/uiComponents/ToothButton.h"


PerioView::PerioView(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

#ifdef Q_OS_WIN
	auto margins = ui.frame_2->layout()->contentsMargins();
	margins.setTop(50);
	ui.frame_2->layout()->setContentsMargins(margins);
	ui.label_20->setMinimumHeight(333);
#endif

    ui.frame->setDynamicFocusBorderChange();

    connect(ui.dateEdit, &QDateEdit::dateChanged, this, [=, this](QDate d) {if (presenter)presenter->dateChanged(Date{ d.day(), d.month(), d.year() });});
	QButtonGroup* group = new QButtonGroup(this);
	group->addButton(ui.upperButton);
	group->addButton(ui.lowerButton);
	group->setExclusive(true);

	ui.lowerButton->setPosition(HalfRoundedButton::Position::Right);

	setFixedHeight(1470);

	ui.upperButton->setChecked(true);
    connect(ui.upperButton, &QPushButton::clicked, this,
        [=, this] {
			ui.stackedWidget->setCurrentWidget(ui.maxilla);
			presenter->teethViewChanged(false); 
		});
    connect(ui.lowerButton, &QPushButton::clicked, this,
        [=, this] {
			ui.stackedWidget->setCurrentWidget(ui.mandibula); 
			presenter->teethViewChanged(true);
		});

	initializeCommon();
	initializeSurfaces();
	initializeRecAndAtt();
	initializeTeethScenes();
	initializeFullMouth();
	
	for (int i = 0; i < 32; i++)
	{
        connect(m_tooth[i], &ToothButton::clicked, this,
            [=, this] { presenter->toothButtonClicked(i); });

        connect(m_furcation[i], &FurcationWidget::valueChanged, this,
            [=, this](FurcationMeasurment m)
			{
				presenter->furcationChanged(i, m.a, m.b, m.c);
			});
	}

	for (int i = 0; i < 64; i++){
        connect(m_Attach[i], QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=, this](int value) {presenter->attachChanged(i, value); });
	}

	for (int i = 0; i < 192; i++)
	{
        connect(m_PD[i], QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=, this](int value) { presenter->pdChanged(i, value); });

        connect(m_CAL[i], QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=, this](int value) { presenter->calChanged(i, value); });

        connect(m_GM[i], QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=, this](int value) { presenter->gmChanged(i, value); });

        connect(m_BOP[i], &QAbstractButton::clicked, this, [=, this] {presenter->bopChanged(i, m_BOP[i]->isChecked()); });
	}


	for(int i = 0; i < 5; i++)
        connect(m_smoke[i], &QRadioButton::clicked, this, [=, this]{if(presenter)presenter->smokeClicked(i); });

    connect(ui.boneSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
        [=, this](int value) { presenter->boneLossChanged(value); } );


    connect(ui.systemicCheck, &QCheckBox::clicked, this,
        [=, this] {presenter->systemicChanged(ui.systemicCheck->isChecked()); });

    connect(ui.restoreCheck, &QCheckBox::clicked, this,
        [=, this] {presenter->restorationChanged(ui.restoreCheck->isChecked()); });

	setStyleSheet(Theme::getFancyStylesheet());

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

	for (int i = index * 6; i < index * 6 + 6; i++)
	{
		refreshChartMeasurment(i);
	}
}

void PerioView::setToothData(const PerioToothData& data)
{
	auto& idx = data.toothIndex;

	auto uiTooth = getUIbyTooth(idx);

	uiTooth.setData(data);

	for (int i = idx * 6; i < idx * 6 + 6; i++)
	{
		refreshChartMeasurment(i);
	}
}


void PerioView::setPerioStatistic(const PerioStatistic& stat)
{
	ui.perioStatistics->setPerioStatistic(stat);
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

	//m_furcation[hint.idx]->setIndex(hint.idx);
	m_tooth[hint.idx]->setText(hint.num.c_str());
}

void PerioView::setAdditional(int smoker, int boneLoss, bool systemic, bool restore)
{
	constexpr int smokeSize = 5;

	for (int i = 0; i < smokeSize; i++)
	{
		QSignalBlocker b(m_smoke[i]);
		m_smoke[i]->setChecked(i == smoker);
	}
	
	QSignalBlocker b(ui.boneSpin);
	ui.boneSpin->setValue(boneLoss);

	ui.systemicCheck->setChecked(systemic);
	ui.restoreCheck->setChecked(restore);
}

void PerioView::showLowerTeeth(bool showLower)
{
	showLower ?
		ui.lowerButton->click() :
		ui.upperButton->click();
}

PatientTileInfo* PerioView::patientTile()
{
	return ui.patientInfoTile;
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

void PerioView::setDate(const Date& d)
{
	QSignalBlocker b(ui.dateEdit);
	ui.dateEdit->setDate(QDate(d.year, d.month, d.day));
}

void PerioView::print()
{
    auto currentStylesheet = styleSheet();

    setStyleSheet("");

    auto getWidgetPixmap = [](QWidget* w){
        w->ensurePolished();
        if (w->layout()) w->layout()->activate();   // optional, helps with lazy layouts

        QPixmap pm(w->size());
        pm.fill(Qt::transparent);

        QPainter p(&pm);
        w->render(&p);                     // paints w + children at current size
        p.end();

        return pm;
    };

    getWidgetPixmap(ui.mandibula).save("widget.png", "PNG");
    getWidgetPixmap(ui.maxilla).save("widget.png", "PNG");

    setStyleSheet(currentStylesheet);

}



void PerioView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Theme::background);
}


ToothUi PerioView::getUIbyTooth(int idx)
{
	ToothUi toothUi;

	toothUi.tooth = m_tooth[idx];
	toothUi.mobi = m_mobi[idx];
	toothUi.furcation = m_furcation[idx];

	toothUi.attachment[0] = m_Attach[idx*2];
	toothUi.attachment[1] = m_Attach[idx*2 + 1];
	toothUi.recession[0] = m_Rec[idx*2];
	toothUi.recession[1] = m_Rec[idx*2 + 1];

	for (int i = 0; i < 6; i++)
	{
		const auto uiIdx = idx * 6 + i;
		toothUi.pd[i] = m_PD[uiIdx];
		toothUi.cal[i] = m_CAL[uiIdx];
		toothUi.gm[i] = m_GM[uiIdx];
		toothUi.bop[i] = m_BOP[uiIdx];
	}

	for (int i = 0; i < 4; i++)
	{
		const auto uiIdx = idx * 4 + i;
		toothUi.FMBS[i] = m_FMBS[uiIdx];
		toothUi.FMPS[i] = m_FMPS[uiIdx];
	}

	return toothUi;

}

void PerioView::refreshChartMeasurment(int idx)
{
	int GM = m_GM[idx]->value();
	int CAL = m_CAL[idx]->value();

	auto& chartIdx = chartIndex[idx];

	perioChart[static_cast<int>(chartIdx.position)]->setMeasurment(chartIdx.index, GM, CAL);
}


//initializations

void PerioView::initializeCommon()
{
	

	//initializing common stuff for maxillary view

	for (int i = 0; i < 16; i++)
	{
		m_tooth[i] = new ToothButton(ui.maxilla);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
		
		ui.maxilla->ui.toothHeaderLayout->addWidget(m_tooth[i]);

		m_mobi[i] = new PerioStateButton(ui.maxilla);
		m_mobi[i]->setAvailableStates(&m_mobiStates);
		m_mobi[i]->setMaximumHeight(20);
		ui.maxilla->ui.mobiLayout->addWidget(m_mobi[i]);


		m_furcation[i] = new FurcationWidget(ui.maxilla);
		m_furcation[i]->setIndex(i);
		ui.maxilla->ui.furcationLayout->addWidget(m_furcation[i]);
	}

	//initializing common stuff for mandibular view
	for (int i = 31; i >= 16; i--)
	{
		m_tooth[i] = new ToothButton(ui.mandibula);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setMinimumWidth(10);

		ui.mandibula->ui.toothHeaderLayout->addWidget(m_tooth[i]);


		m_mobi[i] = new PerioStateButton(ui.mandibula);
		m_mobi[i]->setAvailableStates(&m_mobiStates);
		m_mobi[i]->setMaximumHeight(20);
		ui.mandibula->ui.mobiLayout->addWidget(m_mobi[i]);


		m_furcation[i] = new FurcationWidget(ui.mandibula);
		m_furcation[i]->setIndex(i);
		ui.mandibula->ui.furcationLayout->addWidget(m_furcation[i]);
	}


	m_smoke[0] = ui.radioSmoke_1;
	m_smoke[1] = ui.radioSmoke_2;
	m_smoke[2] = ui.radioSmoke_3;
	m_smoke[3] = ui.radioSmoke_4;
	m_smoke[4] = ui.radioSmoke_5;

	m_tooth[0]->setLayoutPos(LayoutPosition::Left);
	m_tooth[15]->setLayoutPos(LayoutPosition::Right);
	m_tooth[16]->setLayoutPos(LayoutPosition::Right);
	m_tooth[31]->setLayoutPos(LayoutPosition::Left);

}

void PerioView::initializeSurfaces()
{

	//initializing PD, CAL, GM for maxillary view:

	int chartArrayIdx = 0;

	//buccal:
	for (int i = 0; i < 93; i++)
	{
		if (!(i % 3) && i) i += 3;
		
		m_PD[i] = new PerioSpinBox(ui.maxilla);
		m_PD[i]->setMaximum(19);
		m_PD[i]->redValue = 5;
		ui.maxilla->ui.pdUpLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(ui.maxilla);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.maxilla->ui.calUpLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(ui.maxilla);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(QIcon{":/icons/icon_BOP.png"});
		ui.maxilla->ui.bopUpLayout->addWidget(m_BOP[i]);

		this->chartIndex[i].position = ChartPosition::maxBuccal;
		this->chartIndex[i].index = chartArrayIdx;
		chartArrayIdx++;
	}

	chartArrayIdx = 0;

	//palatal:
	for (int i = 3; i < 96; i++)
	{
		if (!(i % 3) && i != 3) i+=3;

		m_PD[i] = new PerioSpinBox(ui.maxilla);
		m_PD[i]->setMaximum(19);
		m_PD[i]->redValue = 5;
		ui.maxilla->ui.pdDownLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(ui.maxilla);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.maxilla->ui.calDownLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(ui.maxilla);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(QIcon{ ":/icons/icon_BOP.png" });
		ui.maxilla->ui.bopDownLayout->addWidget(m_BOP[i]);

		this->chartIndex[i].position = ChartPosition::maxPalatal;
		this->chartIndex[i].index = chartArrayIdx;
		chartArrayIdx++;
	}

	for (int i = 0; i < 93; i++)
	{
		if (!(i % 3) && i) i += 3;

		m_GM[i] = new PerioSpinBox(ui.maxilla);
		m_GM[i]->setRange(-19, 19);
		ui.maxilla->ui.gmUpLayout->addWidget(m_GM[i]);
	}

	for (int i = 3; i < 96; i++)
	{
		if (!(i % 3) && i != 3) i += 3;

		m_GM[i] = new PerioSpinBox(ui.maxilla);
		m_GM[i]->setRange(-19, 19);
		ui.maxilla->ui.gmDownLayout->addWidget(m_GM[i]);
	}

	//now the mandibular view :@ :@ :@ :@

	chartArrayIdx = 48;

	//buccal:

	for (int i = 188; i >= 96; i--)
	{
		//the buccal row:
		m_PD[i] = new PerioSpinBox(ui.mandibula);
		m_PD[i]->setMaximum(19);
		m_PD[i]->redValue = 5;
		ui.mandibula->ui.pdUpLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(ui.maxilla);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.mandibula->ui.calUpLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(ui.mandibula);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(QIcon{ ":/icons/icon_BOP.png" });
		ui.mandibula->ui.bopUpLayout->addWidget(m_BOP[i]);

		this->chartIndex[i].position = ChartPosition::mandLing;
		this->chartIndex[i].index = chartArrayIdx-1;
		chartArrayIdx--;

		if (!(i % 3) && i != 188) i -= 3;
	}

	//lingual:

	chartArrayIdx = 48;

	for (int i = 191; i >= 99; i-- )
	{
		//the lingual row:

		m_PD[i] = new PerioSpinBox(ui.mandibula);
		m_PD[i]->setMaximum(19);
		m_PD[i]->redValue = 5;
		ui.mandibula->ui.pdDownLayout->addWidget(m_PD[i]);

		m_CAL[i] = new PerioSpinBox(ui.mandibula);
		m_CAL[i]->setMaximum(19);
		m_CAL[i]->redValue = 4;
		ui.mandibula->ui.calDownLayout->addWidget(m_CAL[i]);

		m_BOP[i] = new PerioButton(ui.mandibula);
		m_BOP[i]->setMinimumWidth(1);
		m_BOP[i]->setCheckable(true);
		m_BOP[i]->setIcon(QIcon{ ":/icons/icon_BOP.png" });
		ui.mandibula->ui.bopDownLayout->addWidget(m_BOP[i]);

		this->chartIndex[i].position = ChartPosition::mandBuccal;
		this->chartIndex[i].index = chartArrayIdx-1;
		chartArrayIdx--;

		if (!(i % 3) && i != 192) i -= 3;

	}

	for (int i = 188; i >= 96; i--)
	{

		m_GM[i] = new PerioSpinBox(ui.mandibula);
		m_GM[i]->setRange(-19, 19);
		ui.mandibula->ui.gmUpLayout->addWidget(m_GM[i]);

		if (!(i % 3) && i != 188) i -= 3;
	}


	for (int i = 191; i >= 99; i--)
	{
		//the lingual row:

		m_GM[i] = new PerioSpinBox(ui.mandibula);
		m_GM[i]->setRange(-19, 19);
		ui.mandibula->ui.gmDownLayout->addWidget(m_GM[i]);

		if (!(i % 3) && i != 192) i -= 3;

	}
}

void PerioView::initializeFullMouth()
{
	QGraphicsScene* maxillaryScene = new QGraphicsScene;
	ui.maxilla->ui.graphicsSurfaceView->setScene(maxillaryScene);

	ui.maxilla->ui.graphicsSurfaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.maxilla->ui.graphicsSurfaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    double xPos = 0;

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

	for (int i = 0; i < 32; i += 2)
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
		m_Attach[i + 1] = new PerioSpinBox(ui.maxilla);
		m_Attach[i + 1]->setMaximum(9);
		ui.maxilla->ui.agDownLayout->addWidget(m_Attach[i + 1]);

		m_Rec[i + 1] = new PerioSpinBox(ui.maxilla);
		m_Rec[i + 1]->setMaximum(19);
		m_Rec[i + 1]->setReadOnly(true);
		m_Rec[i + 1]->redValue = 1;
		ui.maxilla->ui.recDownLayout->addWidget(m_Rec[i + 1]);
	}

	//recession and attachment sponboxes for mandibula:

	for (int i = 63; i >= 32; i -= 2)
	{
		//lingual:
		m_Attach[i-1] = new PerioSpinBox(ui.mandibula);
		m_Attach[i-1]->setMaximum(9);
		ui.mandibula->ui.agUpLayout->addWidget(m_Attach[i-1]);

		m_Rec[i-1] = new PerioSpinBox(ui.mandibula);
		m_Rec[i-1]->setMaximum(19);
		m_Rec[i-1]->setReadOnly(true);
		m_Rec[i-1]->redValue = 1;
		ui.mandibula->ui.recUpLayout->addWidget(m_Rec[i-1]);

		//buccal:
		m_Attach[i] = new PerioSpinBox(ui.mandibula);
		m_Attach[i]->setMaximum(9);
		ui.mandibula->ui.agDownLayout->addWidget(m_Attach[i]);

		m_Rec[i] = new PerioSpinBox(ui.mandibula);
		m_Rec[i]->setMaximum(19);
		m_Rec[i]->setReadOnly(true);
		m_Rec[i]->redValue = 1;
		ui.mandibula->ui.recDownLayout->addWidget(m_Rec[i]);
	}

	//palatal surfaces have no attached gingiva!
	for (int i = 1; i < 32; i += 2)
		m_Attach[i]->disable(true);
}

void PerioView::initializeTeethScenes()
{
	//scene maxilaris:
	perioScene[0] = new PerioScene();

	perioChart[maxBuccal] = new PerioChartItem();
	perioScene[0]->addItem(perioChart[maxBuccal]);
	perioChart[maxBuccal]->setPos(0, 0);

	perioChart[maxPalatal] = new PerioChartItem();
	perioScene[0]->addItem(perioChart[maxPalatal]);

	perioChart[maxPalatal]->setTransform(QTransform::fromScale(-1, 1));
	perioChart[maxPalatal]->setRotation(180);
	perioChart[maxPalatal]->setPos(0, 340);

	ui.maxilla->ui.graphicsView->setScene(perioScene[0]);

	//scene mandibularis:
	perioScene[1] = new PerioScene();

	perioChart[mandBuccal] = new PerioChartItem();
	perioScene[1]->addItem(perioChart[mandBuccal]);

	perioChart[mandBuccal]->setTransform(QTransform::fromScale(-1, -1));
	perioChart[mandBuccal]->setPos(perioChart[mandBuccal]->boundingRect().width(), 332);

	perioChart[mandLing] = new PerioChartItem();
	perioScene[1]->addItem(perioChart[mandLing]);

	perioChart[mandLing]->setRotation(180);
	perioChart[mandLing]->setTransform(QTransform::fromScale(1, -1));
	perioChart[mandLing]->setPos(perioChart[mandLing]->boundingRect().width(), -5);

	ui.mandibula->ui.graphicsView->setScene(perioScene[1]);

}
