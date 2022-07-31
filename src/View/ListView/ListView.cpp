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
	ui.teethView->installEventFilter(this);
	ui.procedureTable->setModel(&model);
	ui.procedureTable->setAmbListLayout();

	ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
	ui.invoiceButton->setIcon(QIcon(":/icons/icon_invoice.png"));
	ui.addProcedure->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteProcedure->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editProcedure->setIcon(QIcon(":/icons/icon_edit.png"));
	ui.showAppliedButton->setIcon(QIcon(":/icons/icon_apply.png"));
	ui.nzokActivities->setIcon(QIcon(":/icons/icon_nzok.png"));

	ui.perioButton->setHoverColor(Theme::mainBackgroundColor);
	ui.invoiceButton->setHoverColor(Theme::mainBackgroundColor);
	ui.addProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.deleteProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.editProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.showAppliedButton->setHoverColor(Theme::mainBackgroundColor);
	ui.nzokActivities->setHoverColor(Theme::mainBackgroundColor);

	ui.procedureTable->setStyleSheet(
		"color :" + Theme::colorToString(Theme::fontTurquoise) + "; "
		"selection-color:" + Theme::colorToString(Theme::fontTurquoiseClicked) + "; "
		"selection-background-color: " + Theme::colorToString(Theme::background) + "; "
	);

	setStyleSheet("QLabel{ color :" + Theme::colorToString(Theme::fontTurquoise) + ";}");

	ui.procedureLabel->setStyleSheet(
		"color : " + Theme::colorToString(Theme::fontTurquoise) + "; "
		"font-weight: bold; font-size: 12px;"
	);

	connect(ui.patientTile->nraIcon, &QPushButton::clicked, [=] {if (presenter)presenter->checkHealthInsurance(true);});
	connect(ui.allergiesTile->nzokIcon, &QPushButton::clicked, [=] {if (presenter)presenter->checkDiagnosisNhif();});
	connect(ui.nzokActivities, &QPushButton::clicked, [=] { if (presenter) presenter->openPisHistory(); });
	connect(ui.patientTile, &QAbstractButton::clicked, [=] { if(presenter) presenter->openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { if (presenter) presenter->openAllergiesDialog(); });
	connect(ui.addProcedure, &QAbstractButton::clicked, [=] { if (presenter) presenter->addProcedure(); });
	connect(ui.procedureTable, &ProcedureTable::deletePressed, [=] { if (presenter) ui.deleteProcedure->click(); });
	connect(ui.unfav_check, &QCheckBox::stateChanged, [=] { if (presenter) presenter->setfullCoverage(ui.unfav_check->isChecked()); });
	connect(ui.editProcedure, &QPushButton::clicked, [=] { if (presenter) presenter->editProcedure(ui.procedureTable->selectedRow()); });
	connect(ui.invoiceButton, &QPushButton::clicked, [=] { if (presenter) presenter->createInvoice(); });
	connect(ui.perioButton, &QPushButton::clicked, [=] { if (presenter) presenter->createPerioMeasurment(); });
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

	connect(ui.showAppliedButton, &QPushButton::clicked, [=] {
			if (presenter) presenter->showCurrentStatus(ui.showAppliedButton->isChecked());
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

	if (!m_teethViewFocused) return;

	painter.setPen(QPen(Theme::mainBackgroundColor));

	auto teethViewPath = Theme::getHalfCurvedPath(
		ui.teethView->width(), 
		ui.teethView->height()
	);

	painter.translate(ui.teethView->pos());
	painter.drawPath(teethViewPath);



}

bool ListView::eventFilter(QObject* obj, QEvent* event)
{
	if (obj != ui.teethView) return false;

	if (event->type() == QEvent::FocusOut)
	{
			m_teethViewFocused = false;
			repaint();

	}
	else if (event->type() == QEvent::FocusIn)
	{
		m_teethViewFocused = true;
		repaint();
	}


	return false;
}


void ListView::refresh(const AmbList& ambList, const Patient& patient)
{
	ui.patientTile->setData(patient, ambList.getDate());
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

	QSignalBlocker b(ui.showAppliedButton);
	ui.showAppliedButton->setChecked(disabled);
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
		result.append("Сума по НЗОК: " + priceToString(NZOKprice) + " ");

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
	
	//this->setFixedHeight(710 + ui.procedureTable->height() + 100);

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
