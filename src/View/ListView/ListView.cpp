#include "ListView.h"

#include "TeethView/ContextMenu.h"
#include "View/Theme.h"


ListView::ListView(QWidget* parent)
	: QWidget(parent), presenter(nullptr)
{

	ui.setupUi(this);
	
	teethViewScene = new TeethViewScene(ui.teethView);
	contextMenu = new ContextMenu();
	teethViewScene->setContextMenu(contextMenu);

	ui.teethView->setScene(teethViewScene);
	ui.teethView->setSceneRect(teethViewScene->sceneRect());

	ui.procedureTable->setModel(&model);
	ui.procedureTable->setAmbListLayout();

	ui.procedureTable->setStyleSheet(
		"color :" + Theme::getRGBStringFromColor(Theme::fontTurquoise) + "; "
		"selection-color:" + Theme::getRGBStringFromColor(Theme::fontTurquoiseClicked) + "; "
		"selection-background-color: " + Theme::getRGBStringFromColor(Theme::background) + "; "
	);

	setStyleSheet("QLabel{ color :" + Theme::getRGBStringFromColor(Theme::fontTurquoise) + ";}");

	ui.procedureLabel->setStyleSheet(
		"color : " + Theme::getRGBStringFromColor(Theme::fontTurquoise) + "; "
		"font-weight: bold; font-size: 12px;"
	);

	connect(ui.patientTile, &QAbstractButton::clicked, [=] { if(presenter) presenter->openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { if (presenter) presenter->openAllergiesDialog(); });
	connect(ui.procedureButton, &QAbstractButton::clicked, [=] { if (presenter) presenter->addProcedure(); });
	connect(ui.procedureTable, &ProcedureTable::deletePressed, [=] { if (presenter) ui.deleteProcedure->click(); });
	connect(ui.unfav_check, &QCheckBox::stateChanged, [=] { if (presenter) presenter->setfullCoverage(ui.unfav_check->isChecked()); });
	connect(ui.editProcedure, &QPushButton::clicked, [=] { if (presenter) presenter->editProcedure(ui.procedureTable->selectedRow()); });
	connect(ui.invoiceButton, &QPushButton::clicked, [=] { if (presenter) presenter->createInvoice(); });
	connect(ui.deleteProcedure, &QAbstractButton::clicked, 
		[=] {

			if (!presenter) return;

			int currentIdx = ui.procedureTable->selectedRow();
			int lastIdx = ui.procedureTable->verticalHeader()->count()-1;

//			if (currentIdx == -1) return;

			presenter->deleteProcedure(currentIdx);

			if (currentIdx == lastIdx)
			{
				ui.procedureTable->selectRow(currentIdx - 1);
			}
			else ui.procedureTable->selectRow(currentIdx);
		});

	connect(ui.procedureTable, &QTableView::doubleClicked, [=] { ui.editProcedure->click(); });
	connect(ui.taxCombo, &QComboBox::currentIndexChanged,
			[=](int index) {presenter->chargeChanged(index); });

	connect(ui.showCurrentStatusBox, &QPushButton::clicked, [=] { 
			if (presenter) presenter->showCurrentStatus(ui.showCurrentStatusBox->isChecked()); 
		});

	ui.controlPanel->hide();
	ui.surfacePanel->hide();

}

void ListView::setPresenter(ListPresenter* presenter)
{
	this->presenter = presenter;
	teethViewScene->setPresenter(presenter);
	ui.controlPanel->setPresenter(presenter);
	contextMenu->setPresenter(presenter);
}

void ListView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);

	QPainterPath path;

	path.addRoundedRect(
		QRectF(
			ui.teethView->x(),
			ui.teethView->y(),
			ui.controlPanel->x() + ui.controlPanel->width(),
			ui.teethView->height()
		),
		Theme::radius/2,
		Theme::radius/2
	);

	painter.fillPath(path, Theme::sectionBackground);
	
	painter.setPen(QPen(Theme::border));
	painter.drawPath(path);

	painter.drawLine(
		ui.surfacePanel->x(),
		ui.surfacePanel->y(),
		ui.surfacePanel->x(),
		ui.surfacePanel->y() + ui.teethView->height()
	);

	painter.end();
}


void ListView::refresh(const AmbList& ambList, const Patient& patient)
{
	ui.patientTile->setData(patient, ambList.getAmbListDate());
	ui.allergiesTile->setData(patient);
	ui.taxCombo->setIndex(static_cast<int>(ambList.charge));
	
}

void ListView::setCheckModel(const CheckModel& checkModel)
{
	ui.controlPanel->setModel(checkModel);
	contextMenu->setModel(checkModel);
}

void ListView::hideSurfacePanel(bool hidden)
{
	ui.surfacePanel->hidePanel(hidden);
	ui.controlPanel->hideCommonButtons(!hidden);
}

void ListView::hideControlPanel(bool hidden)
{
	ui.controlPanel->setHidden(hidden);
}

ISurfacePanel* ListView::surfacePanel()
{
	return ui.surfacePanel;
}

void ListView::repaintTooth(const ToothPaintHint& tooth)
{
	teethViewScene->display(tooth);
	ui.teethView->setFocus();
}

void ListView::setNotes(const std::array<std::string, 32>& notes)
{
	teethViewScene->setNotes(notes);
}

void ListView::disableGraphicsView(bool disabled)
{
	teethViewScene->setSelectedTeeth({});
	ui.teethView->setDisabled(disabled);

	QSignalBlocker b(ui.showCurrentStatusBox);
	ui.showCurrentStatusBox->setChecked(disabled);
}


void ListView::setSelectedTeeth(std::vector<int> selectedIndexes)
{

	teethViewScene->setSelectedTeeth(selectedIndexes);

	ui.teethView->update(); //the only way to update qgraphicsview without most of the bugs

}

#include "View/GlobalFunctions.h"

QString getPricesText(double patientPrice, double NZOKprice)
{
	QString result;

	//if (!patientPrice && !NZOKprice) return result;

	//if (NZOKprice)
		result.append("Сума по НЗОК: " + priceToString(NZOKprice) + "<br>");

	result.append("Сума за плащане: " + priceToString(patientPrice));

	return result;
}

void ListView::setProcedures(const std::vector<Procedure>& m, double patientPrice, double nzokPrice)
{
	model.setProcedures(m);

	std::vector<int> proc_teeth;
	proc_teeth.reserve(m.size());

	for (auto t : m) proc_teeth.push_back(t.tooth);

	teethViewScene->setProcedures(proc_teeth);
	
	this->setFixedHeight(710 + ui.procedureTable->height() + 100);

	ui.priceLabel->setText(getPricesText(patientPrice, nzokPrice));
}

AbstractComboBox* ListView::taxCombo()
{
	return ui.taxCombo;
}

void ListView::setUnfav(bool unfav)
{
	QSignalBlocker blocker(ui.unfav_check);
	ui.unfav_check->setChecked(unfav);
}


ListView::~ListView()
{
	delete teethViewScene;
	delete contextMenu;
}
