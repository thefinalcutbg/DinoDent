#include "ListView.h"
#include "Presenter/ListPresenter/StatusPresenter/StatusPresenter.h"
#include "Presenter/ListPresenter/ProcedurePresenter/ProcedurePresenter.h"
#include "TeethView/ContextMenu.h"

ListView::ListView(QWidget* parent)
	: QWidget(parent), presenter(nullptr), procedure_presenter(nullptr)
{

	ui.setupUi(this);
	
	teethViewScene = new TeethViewScene(ui.teethView);
	contextMenu = new ContextMenu();
	teethViewScene->setContextMenu(contextMenu);

	ui.teethView->setScene(teethViewScene);
	ui.teethView->setSceneRect(teethViewScene->sceneRect());
	

	ui.procedureTable->setModel(&model);
	ui.procedureTable->setDimensions();

	connect(ui.patientTile, &QAbstractButton::clicked, [=] { presenter->openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { presenter->openAllergiesDialog(); });
	connect(ui.procedureButton, &QAbstractButton::clicked, [=] { procedure_presenter->addProcedure(); });
	connect(ui.procedureTable, &ProcedureTable::deletePressed, [=] { ui.deleteProcedure->click(); });
	connect(ui.unfav_check, &QCheckBox::stateChanged, [=] {procedure_presenter->setUnfavourable(ui.unfav_check->isChecked()); });
	connect(ui.editProcedure, &QPushButton::clicked, [=] {procedure_presenter->editProcedure(); });

	connect(ui.deleteProcedure, &QAbstractButton::clicked, 
		[=] {
			int currentIdx = ui.procedureTable->selectionModel()->currentIndex().row();
			int lastIdx = ui.procedureTable->verticalHeader()->count()-1;

			if (currentIdx == -1) return;

			procedure_presenter->deleteProcedure(currentIdx);

			if (currentIdx == lastIdx)
			{
				ui.procedureTable->selectRow(currentIdx - 1);
			}
			else ui.procedureTable->selectRow(currentIdx);
		});

	connect(ui.procedureTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, 
		[=] 
		{
			int row = ui.procedureTable->selectionModel()->currentIndex().row();

			if (row == -1) {

				procedure_presenter->setSelectedProcedure(row);
				return;
			}

			int manipulationIdx = ui.procedureTable->model()->index(row, 0).data().toInt();
			procedure_presenter->setSelectedProcedure(row);
		}
	);

	connect(ui.procedureTable, &QTableView::doubleClicked, [=] { procedure_presenter->editProcedure(); });
	connect(ui.taxCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[=](int index) {presenter->chargeChanged(index); });

	connect(ui.dateEdit, &QDateEdit::dateChanged,
		[=]
		{
			auto date = ui.dateEdit->date();
			procedure_presenter->ambDateChanged({ date.day(), date.month(), date.year() });
		});

}

void ListView::setPresenter(ListPresenter* presenter)
{
	this->presenter = presenter;
}

void ListView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);

	painter.end();
}


void ListView::setStatusControlPresenter(StatusPresenter* presenter)
{
	teethViewScene->setPresenter(presenter);
	ui.controlPanel->setPresenter(presenter);
	contextMenu->setStatusPresenter(presenter);
}

void ListView::setProcedurePresenter(ProcedurePresenter* presenter)
{
	procedure_presenter = presenter;
	contextMenu->setProcedurePresenter(presenter);
}

void ListView::refresh(AmbList& ambList, Patient& patient)
{
	ui.patientTile->setPatient(patient);
	ui.allergiesTile->setPatient(patient);
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

void ListView::repaintBridges(const BridgesPaintHint& bridges)
{
	teethViewScene->display(bridges);
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

void ListView::setProcedures(const std::vector<ProcedureRowData>& m, double patientPrice, double nzokPrice)
{
	model.setProcedure(m);

	teethViewScene->setProcedures(m);
	
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
