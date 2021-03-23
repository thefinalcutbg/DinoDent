#include "ProcedureDialog.h"

ProcedureDialog::ProcedureDialog(QWidget *parent)
	: QDialog(parent), presenter(this)
{
	ui.setupUi(this);

	auto table = ui.tableView;

	proxyModel = new QSortFilterProxyModel(this);

	proxyModel->setSourceModel(&model);
	proxyModel->setFilterKeyColumn(2);
	

	table->setModel(proxyModel);
	table->hideColumn(0);
	table->setColumnWidth(0, 20);
	table->setColumnWidth(1, 20);
	table->setColumnWidth(2, 400);
	table->setColumnWidth(3, 70);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);

	QItemSelectionModel* sm = table->selectionModel();

	{
		ProcedureDialogElements p;
		p.dateField = ui.dateEdit;
		p.diagnosisField = ui.diagnosisEdit;
		p.manipulationField = ui.manipulationEdit;
		p.materialField = ui.materialEdit;
		p.rangeBox = ui.rangeWidget;
		p.surfaceSelector = ui.surfaceSelector;
		presenter.setUIComponents(p);
	}




	connect(sm, &QItemSelectionModel::currentRowChanged, this, [=] { 

		int row =  table->selectionModel()->currentIndex().row();

		if (row == -1){
			presenter.indexChanged(row);
			return;
		}

		int manipulationIdx = proxyModel->index(row, 0).data().toInt();
		presenter.indexChanged(manipulationIdx);
		});

	connect(ui.diagnosisEdit, &QTextEdit::textChanged, [=] {presenter.diagnosisChanged(ui.diagnosisEdit->getText()); });
	connect(ui.rangeWidget, &RangeWidget::rangeChanged, [=](int begin, int end) {presenter.rangeChanged(begin, end); });
	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });
	connect(ui.okButton, &QPushButton::clicked, [=] { presenter.formAccepted(); });

	connect(ui.searchEdit, &QLineEdit::textChanged, [=]
		{
			proxyModel->setFilterRegExp(QRegExp(ui.searchEdit->text(), Qt::CaseInsensitive, QRegExp::FixedString));
			ui.tableView->selectRow(0);

		});
}

ProcedureDialog::~ProcedureDialog()
{
}

void ProcedureDialog::openProcedureDialog()
{
	ui.searchEdit->setText("");
	ui.searchEdit->setFocus();
	ui.tableView->selectRow(0);
	exec();

}

void ProcedureDialog::loadManipulationList(std::vector<ManipulationTemplate> manipulationList)
{
	model.setManipulations(manipulationList);
}

void ProcedureDialog::setParameters(double price)
{
	ui.priceSpinBox->setValue(price);
}

void ProcedureDialog::setParameters(const std::string& material)
{
	ui.materialEdit->setText(QString::fromStdString(material));
}

void ProcedureDialog::setSelectionLabel(const std::vector<int>& selectedTeethNum)
{
	QString label;

	label.reserve(15 + selectedTeethNum.size() * 3);

	label.append("Избрани зъби: ");

	for (int i = 0; i < selectedTeethNum.size(); i++)
	{
		label.append(QString::number(selectedTeethNum[i]));
		if (i < selectedTeethNum.size() - 1)
		{
			label.append(", ");
		}
	}

	if (!selectedTeethNum.size())
	{
		label.append("Няма");
	}

	ui.selectedTeethLabel->setText(label);
}

double ProcedureDialog::getPrice()
{
	return ui.priceSpinBox->value();
}


void ProcedureDialog::resetForm()
{
	ui.mainGroup->hide();
	ui.rangeWidget->hide();
	ui.materialEdit->hide();
	ui.materialLabel->hide();
	ui.surfaceSelector->hide();
	ui.noTeethLabel->hide();

	//ui.dateEdit->setAppearence(true);
	ui.manipulationEdit->setAppearence(true);
	ui.diagnosisEdit->setAppearence(true);

}

void ProcedureDialog::setView(ManipulationType t)
{
	resetForm();

	ui.mainGroup->show();

	switch (t)
	{	
	case ManipulationType::obturation:
		ui.materialEdit->show();
		ui.materialLabel->show();
		ui.surfaceSelector->show();
		ui.surfaceSelector->setAppearence(true);
		break;
	case ManipulationType::bridge:
		ui.materialEdit->show();
		ui.materialLabel->show();
		ui.rangeWidget->show();
		break;
	case ManipulationType::extraction:
		break;
	case ManipulationType::general:
		break;
	default:
		ui.materialEdit->show();
		ui.materialLabel->show();
		break;
	}
}

void ProcedureDialog::close()
{
	this->hide();
}

void ProcedureDialog::showErrorMessage(const std::string& error)
{
	resetForm();
	ui.noTeethLabel->show();
	ui.noTeethLabel->setText(QString::fromStdString(error));
}

void ProcedureDialog::showErrorDialog(const std::string& error)
{
	QMessageBox m;
	m.setText(QString::fromStdString(error));
	m.setIcon(QMessageBox::Warning);
	m.exec();
}


ProcedureDialogPresenter* ProcedureDialog::getPresenter()
{
	return &presenter;
}
