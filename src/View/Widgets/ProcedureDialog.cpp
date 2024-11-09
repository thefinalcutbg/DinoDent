#include "ProcedureDialog.h"
#include "Presenter/ProcedureDialogPresenter.h"

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter& presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter), proxyModel(this)
{
	ui.setupUi(this);

	setModal(true);
	
	setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
								   Qt::WindowMaximizeButtonHint |
								   Qt::WindowCloseButtonHint);

	setWindowTitle("Добавяне на манипулация");

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(3);

	auto& table = ui.tableView;

	table->setModel(&proxyModel);
	table->setMouseTracking(true);

	table->setColumnWidth(0, 30);
	table->hideColumn(1);
	table->setColumnWidth(2, 70);
	table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);

	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);


	table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	table->verticalHeader()->hide();

	auto favDelagete = new FavButtonDelegate();
	table->setItemDelegateForColumn(0, favDelagete);

	connect(favDelagete, &FavButtonDelegate::updateRequested, this, [&] {
		ui.tableView->viewport()->update();
	});

	connect(favDelagete, &FavButtonDelegate::favouriteClicked, this, [&](int rowIdx) {
		
		auto code = model.index(rowIdx, 1).data().toString().toStdString();

		presenter.favouriteClicked(code);
		
	});

    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {

            s_idx = table->selectionModel()->currentIndex().row();

            if (s_idx == -1){
				presenter.setCode(ProcedureCode{}, 0);
				ui.tableView->clearSelection(); //because of bug when reseting the model
                return;
            }

            presenter.setCode(
				ProcedureCode(proxyModel.index(s_idx, 1).data().toString().toStdString()),
				proxyModel.index(s_idx, 0).data(Qt::UserRole) == 1 //NHIF value from model
			);
		});

    connect(ui.cancelButton, &QPushButton::clicked, this, [&] { close(); });
    connect(ui.okButton, &QPushButton::clicked, this, [&] { presenter.formAccepted(); });

    connect(ui.searchEdit, &QLineEdit::textChanged, this, [=, this]
		{
			s_search = ui.searchEdit->text();
			proxyModel.setFilterRegularExpression(QRegularExpression(s_search, QRegularExpression::CaseInsensitiveOption));
			ui.tableView->selectRow(s_idx);
		});


    connect(ui.tableView, &QTableView::doubleClicked, this, [&] { presenter.formAccepted(); });

    connect(ui.procedureInput->qDateEdit(), &QDateEdit::dateChanged, this, [&] {

		auto date = ui.procedureInput->qDateEdit()->date();
        presenter.procedureDateChanged(Date{ date.day(), date.month(), date.year() });
        
	});

	connect(ui.sectionCombo, &QComboBox::currentIndexChanged, this, [&](int index) {
		presenter.sectionChanged(index);
	});

	ui.searchEdit->setText(s_search);
	ui.tableView->selectRow(s_idx);

    presenter.setView(this);


}

ProcedureDialog::~ProcedureDialog()
{
}


void ProcedureDialog::setProcedureSections(const std::vector<std::string>& sectionNames, int defaultIdx)
{
	ui.sectionCombo->clear();

	ui.sectionCombo->blockSignals(true);

	for (auto& name : sectionNames) {
		ui.sectionCombo->addItem(name.c_str());
	}

	ui.sectionCombo->setItemIcon(1, QIcon(":/icons/icon_fav.png"));

	ui.sectionCombo->setCurrentIndex(defaultIdx);

	ui.sectionCombo->blockSignals(false);

	emit ui.sectionCombo->currentIndexChanged(defaultIdx);
}

void ProcedureDialog::setProcedureTemplates(std::vector<ProcedureListElement> procedureList)
{
	model.setProcedures(procedureList, ui.sectionCombo->currentIndex() < 2);	
	presenter.setCode(ProcedureCode(), false);
}

void ProcedureDialog::setSelectionLabel(const std::vector<int>& selectedTeethNum)
{
	QString selectedTeeth;

	selectedTeeth.reserve(15 + selectedTeethNum.size() * 3);

	selectedTeeth.append("Избрани зъби: ");

    for (size_t i = 0; i < selectedTeethNum.size(); i++)
	{
		selectedTeeth.append(QString::number(selectedTeethNum[i]));
		if (i < selectedTeethNum.size() - 1)
		{
			selectedTeeth.append(", ");
		}
	}

	if (!selectedTeethNum.size())
	{
		selectedTeeth.append("Няма");
	}

	ui.statusLabel->setText(selectedTeeth);
}

IProcedureInput* ProcedureDialog::procedureInput()
{
	return ui.procedureInput;
}


void ProcedureDialog::close() { QDialog::accept(); }



