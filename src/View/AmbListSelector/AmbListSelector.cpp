#include "AmbListSelector.h"
#include "Presenter/ListSelector/ListSelectorPresenter.h"
#include <QMessageBox>
AmbListSelector::AmbListSelector(ListSelectorPresenter* presenter) :
	p(presenter)
{
	ui.setupUi(this);
	QWidget::setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window);
	//setWindowFlags(Qt::WindowStaysOnTopHint);

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(3);

	ui.tableView->setModel(&model);

	ui.tableView->setDimensions();

	connect(ui.monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {presenter->setDate(index + 1, ui.yearCombo->currentText().toInt()); });
	connect(ui.yearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {presenter->setDate(ui.monthCombo->currentIndex() + 1, ui.yearCombo->currentText().toInt()); });
	connect(ui.tableView, &QTableView::doubleClicked, this, [=] { presenter->openAmbList(); });
	connect(ui.tableView, &ListTable::deletePressed, this, [=] { ui.deleteButton->click(); });
	connect(ui.openButton, &QPushButton::clicked, [=] {presenter->openAmbList(); });

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedIndexes();
		
		std::vector<int>selectedIndexes;

		for (auto& i : idxList)
			selectedIndexes.push_back(i.row());

		presenter->selectionChanged(selectedIndexes);

		});

	connect(ui.deleteButton, &QPushButton::clicked, 
		[=] 
		{
			QMessageBox msgBox;
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setText("Сигурни ли сте, че искате да изтриете амбулаторния лист?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setButtonText(QMessageBox::Yes, "Да");
			msgBox.setButtonText(QMessageBox::No, "Не");
			msgBox.setDefaultButton(QMessageBox::No);
			int answer = msgBox.exec();

			if(answer == QMessageBox::Yes)
				presenter->deleteAmbList();

		});

	presenter->setView(this);
}

AmbListSelector::~AmbListSelector()
{
	ui.monthCombo->blockSignals(true);
	ui.yearCombo->blockSignals(true);
	p->setView(nullptr);

}

void AmbListSelector::addYearsToCombo(const std::vector<int>& years)
{
	QSignalBlocker b(ui.yearCombo);

	ui.yearCombo->clear();

	for (auto& y: years)
	{
		ui.yearCombo->addItem(QString::number(y));
	}
	
	
}

void AmbListSelector::setUI(int month, int year)
{
	QString selected_year = QString::number(year);
	QSignalBlocker b(ui.monthCombo);
	ui.monthCombo->setCurrentIndex(month - 1);

	for (int i = 0; i < ui.yearCombo->count()-1; i++)
	{
		if (ui.yearCombo->itemText(i) == selected_year)
		{
			QSignalBlocker blocker(ui.yearCombo);
			ui.yearCombo->setCurrentIndex(i);
			return;
		}
	}
}

void AmbListSelector::setRows(const std::vector<AmbListRow>& rows)
{
	model.setRows(rows);
}

void AmbListSelector::focus()
{
	activateWindow();
}

void AmbListSelector::close()
{
	QDialog::accept();
}

