#include "ListView.h"
#include "Presenter/ListPresenter/StatusPresenter/StatusPresenter.h"
#include "Presenter/ListPresenter/ProcedurePresenter/ProcedurePresenter.h"
#include "TeethView/ContextMenu.h"

ListView::ListView(QWidget* parent)
	: QWidget(parent), allergiesDialog(this)
{

	ui.setupUi(this);

	teethViewScene = new TeethViewScene(ui.teethView);
	contextMenu = new ContextMenu();
	teethViewScene->setContextMenu(contextMenu);

	ui.teethView->setScene(teethViewScene);
	ui.teethView->setSceneRect(teethViewScene->sceneRect());
	presenter.setDialogPresnters(allergiesDialog.getPresenter());

	ui.procedureTable->setModel(&model);
	ui.procedureTable->setDimensions();

	connect(ui.patientTile, &QAbstractButton::clicked, [=] { presenter.openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { presenter.openAllergiesDialog(); });
	connect(ui.procedureButton, &QAbstractButton::clicked, [=] { procedure_presenter->addProcedure(); });
	connect(ui.procedureTable, &ProcedureTable::deletePressed, [=] { ui.deleteProcedure->click(); });
	connect(ui.unfav_check, &QCheckBox::stateChanged, [=] {procedure_presenter->setUnfavourable(ui.unfav_check->isChecked()); });

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

	presenter.setView(this);

}

void ListView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);

	painter.end();
}

ListPresenter* ListView::Presenter()
{
	return &this->presenter;
}

void ListView::setStatusControlPresenter(StatusPresenter* presenter)
{
	teethViewScene->setPresenter(presenter);
	ui.controlPanel->setPresenter(presenter);
	contextMenu->setStatusControl(presenter);
}

void ListView::setProcedurePresenter(ProcedurePresenter* presenter)
{
	procedure_presenter = presenter;
}

void ListView::refresh(AmbList& ambList, Patient& patient)
{
	ui.patientTile->setPatient(patient);
	ui.allergiesTile->setPatient(patient);
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
	{
		QSignalBlocker blocker(teethViewScene);
		teethViewScene->setSelectedTeeth(selectedIndexes);
	}
	//ui.teethView->setFocus();
	ui.teethView->update(); //the only way to update qgraphicsview without most of the bugs
}

void ListView::setProcedures(const std::vector<RowData>& m)
{
	model.setProcedure(m);

	teethViewScene->setProcedures(m);
	
	int tableHeight = m.size() * 50 + 26;
	//ne sym siguren izob6to, 4e taka iskam da izglejda:
	auto size = ui.procedureTable->size();
	size.setHeight(tableHeight);
	ui.procedureTable->setFixedSize(size);
	this->setFixedHeight(710 + tableHeight + 100);

	if (!m.size())
	{
		ui.procedureTable->hide();

	}
	else
	{
		ui.procedureTable->show();
	}

	
}

void ListView::openProcedureDialog(ProcedureDialogPresenter *p)
{
	ProcedureDialog dialog(p);
	dialog.openProcedureDialog();
}

void ListView::setUnfav(bool unfav)
{
	QSignalBlocker blocker(ui.unfav_check);
	ui.unfav_check->setChecked(unfav);
	
}


ListView::~ListView()
{
	ui.teethView->blockSignals(true); //have to fix setting selected teeth to non-existing entity! (ListInstance vector throws exception)
	
	delete teethViewScene;
	delete contextMenu;
}
