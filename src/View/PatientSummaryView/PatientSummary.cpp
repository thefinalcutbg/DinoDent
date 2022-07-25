#include "PatientSummary.h"
#include <QPainter>
#include "Presenter/PatientSummaryPresenter/PatientSummaryPresenter.h"
#include "View/Theme.h"
#include <QButtonGroup>

PatientSummary::PatientSummary(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);

	ui.openButton->setIcon(QIcon(":/icons/icon_open.png"));
	setStyleSheet("QLabel { color :" + Theme::colorToString(Theme::fontTurquoise) + ";}");

	connect(ui.patientTile, &QAbstractButton::clicked, [=] { if (presenter) presenter->openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { if (presenter) presenter->openAllergiesDialog(); });
	connect(ui.openButton, &QAbstractButton::clicked, [=] {if (presenter) presenter->openCurrentDocument();});
	QButtonGroup* group = new QButtonGroup(this);
	group->addButton(ui.showBuccal);
	group->addButton(ui.showLingual);
	group->setExclusive(true);

	ui.showLingual->setReversed();

	ui.showBuccal->setChecked(true);

	emptyScene = new QGraphicsScene();
	buccalScene = new TeethBuccalScene();
	lingualScene = new TeethLingualScene();

	ui.teethView->setScene(buccalScene);
	ui.teethView->disableMultiSelection();

	ui.procedureTable->setModel(&m_procedureModel);
	ui.procedureTable->setProcedureHistoryLayout();
	ui.dateSlider->setTickPosition(QSlider::TickPosition::TicksBelow);

	auto getDateFromView = [=] {
		auto f = ui.dateFrom->date();
		auto t = ui.dateTo->date();

		presenter->pricePeriodChanged(
			Date(f.day(), f.month(), f.year()),
			Date(t.day(), t.month(), t.year())
		);
	};

	connect(ui.dateFrom, &QDateEdit::dateChanged, [=]() { getDateFromView();});
	connect(ui.dateTo, &QDateEdit::dateChanged, [=]() { getDateFromView();});

	connect(buccalScene, &QGraphicsScene::selectionChanged, [=] {
			auto idx = buccalScene->selectedTooth();
			lingualScene->setSelectedTooth(idx);
			presenter->toothSelected(idx);
		});

	connect(lingualScene, &QGraphicsScene::selectionChanged, [=] {
			auto idx = lingualScene->selectedTooth();
			buccalScene->setSelectedTooth(idx);
			presenter->toothSelected(idx);
		});


	connect(ui.dateSlider, &QSlider::valueChanged, this, [=] {presenter->setCurrentFrame(ui.dateSlider->value());});

	connect(ui.showLingual, &QPushButton::clicked, this,
		[=] {
				ui.teethView->setScene(lingualScene);
		}
	);

	connect(ui.showBuccal, &QPushButton::clicked, this,
		[=] {
			ui.teethView->setScene(buccalScene);
		}
	);

	connect(ui.showPerio, &QCheckBox::stateChanged, this,
		[=] {
				bool show = ui.showPerio->isChecked();
				lingualScene->showPerio(show);
				buccalScene->showPerio(show);

		}
	);

	ui.showPerio->setChecked(true);
	lingualScene->showPerio(true);
	buccalScene->showPerio(true);
}

PatientSummary::~PatientSummary()
{

}

void PatientSummary::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);

	QPainterPath path;

	const QRect r{
			ui.teethView->x(),
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

	if (!ui.teethView->hasFocus()) return;

	painter.setPen(QPen(Theme::mainBackgroundColor));

	auto teethViewPath = Theme::getHalfCurvedPath(
		ui.teethView->width(),
		ui.teethView->height()
	);

	painter.translate(ui.teethView->pos());
	painter.drawPath(teethViewPath);

	painter.end();
}

void PatientSummary::setPresenter(PatientSummaryPresenter* presenter)
{
	this->presenter = presenter;
}

void PatientSummary::setTickPosition(int idx)
{
	ui.dateSlider->setValue(idx);
}


void PatientSummary::setTimeFrameCount(int count)
{
	bool noData = count == 0;

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
	else
	{
		//THE STATE SHOULD BE KEPT IN THE PRESENTER!!!!
		ui.showBuccal->isChecked() ?
			ui.teethView->setScene(buccalScene)
			:
			ui.teethView->setScene(lingualScene);
	}

	ui.dateSlider->setRange(0, count - 1);
	
}

void PatientSummary::setPatient(const Patient& patient)
{
	ui.patientTile->setData(patient, Date::currentDate());
	ui.allergiesTile->setData(patient);
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

	for (auto& procedure : p)
		if (procedure.tooth != -1)
			treatedTeeth.push_back(procedure.tooth);


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
	ui.docDate->setText(u8"Дата: " + QString(date.c_str()));
	ui.docDoctor->setText(u8"Създаден от: " + QString(doctor.c_str()));
}

void PatientSummary::setToothInfo(const ToothInfoStr& info)
{
	ui.vStack->setCurrentIndex(1);
	ui.toothLabel->setText(info.label.c_str());
	ui.toothInfo->setText(info.info.c_str());

	if (info.notes.size()) {
		ui.toothNotes->setText(
			u8"<b>Бележки:</b><br>" +
			QString::fromStdString(info.notes)
		);
	}
	else
	{
		ui.toothNotes->clear();
	}

}

void PatientSummary::setSelectedTooth(int toothIdx)
{
	if (toothIdx == -1) {
		ui.vStack->setCurrentIndex(0);
	}
	buccalScene->setSelectedTooth(toothIdx);
	lingualScene->setSelectedTooth(toothIdx);
}

#include "View/GlobalFunctions.h"

void PatientSummary::setPrice(const PriceInfoStr& p)
{
	QSignalBlocker b1(ui.dateFrom);
	QSignalBlocker b2(ui.dateTo);

	ui.dateFrom->setDate(QDate(p.from.year, p.from.month, p.from.day));
	ui.dateTo->setDate(QDate(p.to.year, p.to.month, p.to.day));

	ui.priceLabel->setText(
		u8"Платени: <b>" + priceToString(p.price) + "</b>" +
		u8" НЗОК: <b>" + priceToString(p.nzok) + "</b>"
	);
}

