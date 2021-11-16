#include "ListView.h"

#include "TeethView/ContextMenu.h"

ListView::ListView(QWidget* parent)
	: QWidget(parent), presenter(nullptr)
{

	ui.setupUi(this);
	
	teethViewScene = new TeethViewScene(ui.teethView);
	contextMenu = new ContextMenu();
	teethViewScene->setContextMenu(contextMenu);

	ui.teethView->setScene(teethViewScene);
	ui.teethView->setSceneRect(teethViewScene->sceneRect());
	
	ui.dateEdit->installEventFilter(this);
	ui.procedureTable->setModel(&model);
	ui.procedureTable->setAmbListLayout();

	connect(ui.patientTile, &QAbstractButton::clicked, [=] { if(presenter) presenter->openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { if (presenter) presenter->openAllergiesDialog(); });
	connect(ui.procedureButton, &QAbstractButton::clicked, [=] { if (presenter) presenter->addProcedure(); });
	connect(ui.procedureTable, &ProcedureTable::deletePressed, [=] { if (presenter) ui.deleteProcedure->click(); });
	connect(ui.unfav_check, &QCheckBox::stateChanged, [=] { if (presenter) presenter->setUnfavourable(ui.unfav_check->isChecked()); });
	connect(ui.editProcedure, &QPushButton::clicked, [=] { if (presenter) presenter->editProcedure(); });

	connect(ui.deleteProcedure, &QAbstractButton::clicked, 
		[=] {

			if (!presenter) return;

			int currentIdx = ui.procedureTable->selectionModel()->currentIndex().row();
			int lastIdx = ui.procedureTable->verticalHeader()->count()-1;

			if (currentIdx == -1) return;

			presenter->deleteProcedure(currentIdx);

			if (currentIdx == lastIdx)
			{
				ui.procedureTable->selectRow(currentIdx - 1);
			}
			else ui.procedureTable->selectRow(currentIdx);
		});

	connect(ui.procedureTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, 
		[=] 
		{
			if (!presenter) return;

			int row = ui.procedureTable->selectionModel()->currentIndex().row();

			if (row == -1) {

				presenter->setSelectedProcedure(row);
				return;
			}

			int manipulationIdx = ui.procedureTable->model()->index(row, 0).data().toInt();
			presenter->setSelectedProcedure(row);
		}
	);

	connect(ui.procedureTable, &QTableView::doubleClicked, [=] { if(presenter) presenter->editProcedure(); });
	connect(ui.taxCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[=](int index) {presenter->chargeChanged(index); });

	connect(ui.dateEdit, &QDateEdit::dateChanged,
		[=]
		{
			if (!presenter) return;
			auto date = ui.dateEdit->date();
			presenter->ambDateChanged({ date.day(), date.month(), date.year() });
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
	painter.fillRect(0, 0, width(), height(), Qt::white);

	painter.end();
}

bool ListView::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Wheel && obj == ui.dateEdit)
	{
		event->ignore();
		return true;
	}

	return false;
}

void ListView::refresh(const AmbList& ambList, const Patient& patient)
{
	ui.patientTile->setData(patient, ambList.date);
	ui.allergiesTile->setData(patient);
	auto& d = ambList.date;
	QSignalBlocker blocker(ui.dateEdit);
	ui.dateEdit->setDate({ d.year, d.month, d.day });
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
		result.append("Сума по НЗОК: " + priceToString(NZOKprice) + "       ");

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
	
	int tableHeight = m.size() * 50 + 26;
	//ne sym siguren izob6to, 4e taka iskam da izglejda:
	auto size = ui.procedureTable->size();
	size.setHeight(tableHeight);
	ui.procedureTable->setFixedSize(size);
	this->setFixedHeight(710 + tableHeight + 100);

	ui.priceLabel->setText(getPricesText(patientPrice, nzokPrice));

	ui.procedureTable->setHidden(!m.size());
	
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
