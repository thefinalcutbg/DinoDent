﻿#include "PatientSummary.h"
#include <QPainter>
#include "Presenter/PatientSummaryPresenter.h"
#include "View/Theme.h"
#include "View/uiComponents/MouseWheelGuard.h"
#include <QButtonGroup>

PatientSummary::PatientSummary(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);

	ui.sheetButton->setIcon(QIcon(":/icons/icon_sheet.png"));
	ui.invoiceButton->setIcon(QIcon(":/icons/icon_invoice.png"));
	ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
	ui.prescrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
	ui.openButton->setIcon(QIcon(":/icons/icon_open.png"));

	ui.sheetButton->setHoverColor(Theme::mainBackgroundColor);
	ui.invoiceButton->setHoverColor(Theme::mainBackgroundColor);
	ui.perioButton->setHoverColor(Theme::mainBackgroundColor);
	ui.prescrButton->setHoverColor(Theme::mainBackgroundColor);
	ui.openButton->setHoverColor(Theme::mainBackgroundColor);

	setStyleSheet("QLabel { color :" + Theme::colorToString(Theme::fontTurquoise) + ";}");

    connect(ui.openButton, &QAbstractButton::clicked, this, [=, this] {if (presenter) presenter->openCurrentDocument();});
    connect(ui.sheetButton, &QAbstractButton::clicked, this, [=, this] {if (presenter) presenter->newAmbSheet();});
    connect(ui.perioButton, &QAbstractButton::clicked, this, [=, this] {if (presenter) presenter->newPerio();});
    connect(ui.prescrButton, &QAbstractButton::clicked, this, [=, this] {if (presenter) presenter->newPrescription();});
    connect(ui.invoiceButton, &QAbstractButton::clicked, this, [=, this] {if (presenter) presenter->newInvoice();});
	
	QButtonGroup* group = new QButtonGroup(this);
	group->addButton(ui.showBuccal);
	group->addButton(ui.showLingual);
	group->setExclusive(true);

	ui.showLingual->setPosition(HalfRoundedButton::Position::Right);

	ui.showBuccal->setChecked(true);

	emptyScene = new QGraphicsScene();
	buccalScene = new TeethBuccalScene();
	lingualScene = new TeethLingualScene();

	ui.teethView->setScene(buccalScene);
	ui.teethView->disableMultiSelection();

	ui.procedureTable->setModel(&m_procedureModel);
	ui.procedureTable->setProcedureHistoryLayout();
	ui.dateSlider->setTickPosition(QSlider::TickPosition::TicksBelow);
	ui.dateSlider->installEventFilter(new MouseWheelGuard(ui.dateSlider));
/*
	auto getDateFromView = [=] {
		auto f = ui.dateFrom->date();
		auto t = ui.dateTo->date();

		if (presenter)
		presenter->pricePeriodChanged(
			Date(f.day(), f.month(), f.year()),
			Date(t.day(), t.month(), t.year())
		);
	};

    connect(ui.dateFrom, &QDateEdit::dateChanged, this, [=, this]() { getDateFromView();});
    connect(ui.dateTo, &QDateEdit::dateChanged, this, [=, this]() { getDateFromView();});
*/

    connect(buccalScene, &QGraphicsScene::selectionChanged, this, [=, this] {
			auto idx = buccalScene->selectedTooth();
			lingualScene->setSelectedTooth(idx);
			if (presenter) presenter->toothSelected(idx);
		});

    connect(lingualScene, &QGraphicsScene::selectionChanged, this, [=, this] {
			auto idx = lingualScene->selectedTooth();
			buccalScene->setSelectedTooth(idx);
			if (presenter) presenter->toothSelected(idx);
		});


    connect(ui.dateSlider, &QSlider::valueChanged, this, [=, this] {presenter->setCurrentFrame(ui.dateSlider->value());});

	connect(ui.showLingual, &QPushButton::clicked, this,
        [=, this] {
				ui.teethView->setScene(lingualScene);
				if (presenter) presenter->teethViewButtonClicked(false);
		}
	);

	connect(ui.showBuccal, &QPushButton::clicked, this,
        [=, this] {
			ui.teethView->setScene(buccalScene);
			if(presenter) presenter->teethViewButtonClicked(true);
		}
	);

	connect(ui.showPerio, &QCheckBox::stateChanged, this,
        [=, this] {
				bool show = ui.showPerio->isChecked();
				lingualScene->showPerio(show);
				buccalScene->showPerio(show);
				if(presenter) presenter->perioCheckBoxClicked(show);
		
		}
	);

	setStyleSheet(Theme::getFancyStylesheet());
}

PatientSummary::~PatientSummary()
{

}

void PatientSummary::paintEvent(QPaintEvent*)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);

	QPainterPath path;

	const QRect r{
			ui.teethView->x()-5,
			ui.teethView->y(),
			ui.teethView->width() + ui.vStack->width(),
			ui.teethView->height()
	};

	path.addRoundedRect(
		r,
		Theme::radius / 2,
		Theme::radius / 2
	);

	painter.fillPath(path, Theme::sectionBackground);

	painter.setPen(QPen(Theme::border));
	painter.drawPath(path);

	const QLine line{
		ui.vStack->x(),
		ui.vStack->y(),
		ui.vStack->x(),
		ui.vStack->y() + ui.teethView->height()
	};

	painter.drawLine(line);
/*
	if (!ui.teethView->hasFocus()) return;

	painter.setPen(QPen(Theme::mainBackgroundColor));

	auto teethViewPath = Theme::getHalfCurvedPath(
		ui.teethView->width(),
		ui.teethView->height()
	);

	painter.translate(ui.teethView->pos());
	painter.drawPath(teethViewPath)
*/
	painter.end();
}

void PatientSummary::setPresenter(PatientSummaryPresenter* presenter)
{
	this->presenter = presenter;
}

IPatientTileInfo* PatientSummary::patientTile()
{
	return ui.patientInfoTile;
}


void PatientSummary::setTeeth(const std::array<ToothPaintHint, 32>& teeth)
{

	for (auto& t : teeth) {
		buccalScene->display(t);
		lingualScene->display(t);
	}

}


void PatientSummary::setPerioData(const PerioWithDisabled& perio)
{
	buccalScene->setMeasurments(perio.pd, perio.cal);
	lingualScene->setMeasurments(perio.pd, perio.cal);
}

void PatientSummary::setInitialAmbList()
{
	ui.hStack->setCurrentIndex(0);
}

void PatientSummary::setProcedures(const std::vector<Procedure>& p)
{
	m_procedureModel.setProcedures(p);

	std::vector<int> treatedTeeth;

	for (auto& procedure : p) {

		auto idx = procedure.getToothIndex();

		if (idx.isValid()) {
			treatedTeeth.push_back(idx.index);
		}
	}

	buccalScene->setProcedures(treatedTeeth);
	lingualScene->setProcedures(treatedTeeth);

	ui.hStack->setCurrentIndex(1);

	update();
}


void PatientSummary::setPerioStatistic(const PerioStatistic& stat)
{
	ui.perioStatisticView->setPerioStatistic(stat);
	ui.hStack->setCurrentIndex(2);

}

void PatientSummary::setDocumentLabel(const std::string& label, const std::string& date, const std::string& doctor)
{
	ui.docName->setText(label.c_str());
	ui.docDate->setText("Дата: " + QString(date.c_str()));
	ui.docDoctor->setText("Създаден от: " + QString(doctor.c_str()));
}

void PatientSummary::setToothInfo(const ToothInfoStr& info)
{
	ui.vStack->setCurrentIndex(1);
	ui.toothLabel->setText(info.label.c_str());
	ui.toothInfo->setText(info.info.c_str());

	if (info.notes.size()) {
		ui.toothNotes->setText(
			"<b><br>Бележки:</b> " +
			QString::fromStdString(info.notes)
		);
	}
	else
	{
		ui.toothNotes->clear();
	}

}

void PatientSummary::hideToothInfo()
{
	ui.vStack->setCurrentIndex(0);
}


/*
#include "View/GlobalFunctions.h"

void PatientSummary::setPrice(const PriceInfoStr& p)
{
	ui.priceLabel->setText(
		"Платени: <b>" + priceToString(p.price) + "</b>" +
		" НЗОК: <b>" + priceToString(p.nhif) + "</b>"
	);
}
*/

void PatientSummary::setUiState(const SummaryState& s)
{
	/*
	//ui.dateTo will still emit the signal
	QSignalBlocker b(ui.dateFrom);
	ui.dateFrom->setDate(QDate(s.from.year, s.from.month, s.from.day));
	ui.dateTo->setDate(QDate(s.to.year, s.to.month, s.to.day));
	*/
	bool noData = s.noData();

	ui.openButton->setHidden(noData);
	ui.dateSlider->setHidden(noData);
	ui.showBuccal->setHidden(noData);
	ui.showLingual->setHidden(noData);
	ui.showPerio->setHidden(noData);

	if (noData) {
		ui.teethView->setScene(emptyScene);
		ui.hStack->setCurrentIndex(0);
		ui.vStack->setCurrentIndex(0);
		ui.docName->setText("Не са открити данни за този пациент");
		ui.docDate->clear();
		ui.docDoctor->clear();
		return;
	}


		if (s.buccalSelected) {
			ui.showBuccal->setChecked(true);
			ui.showBuccal->click();
		}
		else {
			ui.showLingual->setChecked(true);
			ui.showLingual->click();
		}
			


	ui.dateSlider->setRange(0, s.frameCount - 1);

	ui.dateSlider->setValue(s.currentIdx);
	presenter->setCurrentFrame(s.currentIdx);

	ui.showPerio->setChecked(s.showPerioGraph);
	lingualScene->showPerio(s.showPerioGraph);
	buccalScene->showPerio(s.showPerioGraph);
	
	if (s.selectedTooth == -1) {
		ui.vStack->setCurrentIndex(0);
		
	}
	
	buccalScene->setSelectedTooth(s.selectedTooth);
	lingualScene->setSelectedTooth(s.selectedTooth);

	
	
}

