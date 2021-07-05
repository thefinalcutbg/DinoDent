#include "AmbListSelector.h"
#include "Presenter/ListSelector/ListSelectorPresenter.h"

AmbListSelector::AmbListSelector(ListSelectorPresenter* p) :
	p(p)
{
	ui.setupUi(this);
	QWidget::setAttribute(Qt::WA_DeleteOnClose);

	connect(ui.monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {p->setDate(index + 1, ui.yearCombo->currentText().toInt()); });
	connect(ui.yearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {p->setDate(ui.monthCombo->currentIndex() + 1, ui.yearCombo->currentText().toInt()); });

	ui.tableView->setModel(&model);

	p->setView(this);
}

AmbListSelector::~AmbListSelector()
{
	ui.monthCombo->blockSignals(true);
	ui.yearCombo->blockSignals(true);
	p->setView(nullptr);
}

void AmbListSelector::addYearToCombo(int year)
{
	QSignalBlocker b(ui.yearCombo);
	ui.yearCombo->addItem(QString::number(year));
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
