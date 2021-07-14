#include "AmbListSelector.h"
#include "Presenter/ListSelector/ListSelectorPresenter.h"
#include <QMessageBox>
#include <QDebug>
AmbListSelector::AmbListSelector(ListSelectorPresenter* presenter) :
	p(presenter)
{
	ui.setupUi(this);
	QWidget::setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window);
	//setWindowFlags(Qt::WindowStaysOnTopHint);

	idFilter.setSourceModel(&model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);

	ui.tableView->setModel(&nameFilter);

	ui.tableView->setDimensions();

	connect(ui.monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {presenter->setDate(index + 1, ui.yearCombo->currentText().toInt()); });
	connect(ui.yearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {presenter->setDate(ui.monthCombo->currentIndex() + 1, ui.yearCombo->currentText().toInt()); });
	connect(ui.tableView, &QTableView::doubleClicked, this, [=] { presenter->openAmbList(); });
	connect(ui.tableView, &ListTable::deletePressed, this, [=] { ui.deleteButton->click(); });
	connect(ui.openButton, &QPushButton::clicked, [=] {presenter->openAmbList(); });

	connect(ui.idSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.idSearchEdit->text();
			idFilter.setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
		});

	connect(ui.nameSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.nameSearchEdit->text();
			nameFilter.setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
		});

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedIndexes();
	
		std::vector<int>selectedIndexes;

		for (auto &idx : idxList)
		{
			selectedIndexes.push_back(nameFilter.index(idx.row(), 0).data().toInt());
		}
			

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

