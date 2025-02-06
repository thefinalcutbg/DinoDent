#include "ProcedurePrintSelectDialog.h"
#include <QPainter>
#include "View/CommonIcon.h"

void ProcedurePrintSelectDialog::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.fillRect(rect(), Qt::white);
}

ProcedurePrintSelectDialog::ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, const std::vector<Referral>& referrals, QWidget* parent)
	: QDialog(parent), model(procedures)
{
	ui.setupUi(this);
	setModal(true);
    setWindowTitle("Избор на процедури");

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	ui.tableView->setModel(&this->model);

	ui.tableView->setFocusPolicy(Qt::NoFocus);
	ui.tableView->setColumnWidth(0, 110);
	ui.tableView->setColumnWidth(1, 200);
    ui.tableView->setColumnWidth(2, 65);
	ui.tableView->setColumnWidth(3, 300);
    ui.tableView->setColumnWidth(4, 100);
    ui.tableView->setColumnWidth(5, 100);
	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	ui.tableView->setShowGrid(false);
	ui.tableView->verticalHeader()->hide();

	ui.patientButton->setIcon(CommonIcon::getPixmap(CommonIcon::PATIENT));
	ui.nhifButton->setIcon(CommonIcon::getPixmap(CommonIcon::NHIF));
	ui.phifButton->setIcon(CommonIcon::getPixmap(CommonIcon::PHIF));

	if (procedures.empty()) {
		ui.tableView->hide();
		ui.groupBox->hide();
		adjustSize();
	}

    connect(ui.okButton, &QPushButton::pressed, this, [=, this]
		{
			auto select = ui.tableView->selectionModel()->selectedIndexes();

			m_selectedRows = model.selectedRows();

			accept();

		});

    connect(ui.patientButton, &QPushButton::clicked, this, [&] { selectFinancingSource(FinancingSource::Patient); });
    connect(ui.nhifButton, &QPushButton::clicked, this, [&] { selectFinancingSource(FinancingSource::NHIF); });
	connect(ui.phifButton, &QPushButton::clicked, this, [&] { selectFinancingSource(FinancingSource::PHIF); });
	connect(ui.allButton, &QPushButton::clicked, this, [&] { model.selectAll(); });
	if (referrals.empty()) ui.referralCheck->setHidden(true);

}

const std::vector<int> ProcedurePrintSelectDialog::selectedProcedures() const
{
	return m_selectedRows;
}

bool ProcedurePrintSelectDialog::printReferrals() const
{
	return !ui.referralCheck->isHidden() && ui.referralCheck->isChecked();
}

void ProcedurePrintSelectDialog::selectFinancingSource(FinancingSource source)
{
	model.selectFinancingSource(source);
	ui.referralCheck->setChecked(source == FinancingSource::NHIF);
}


ProcedurePrintSelectDialog::~ProcedurePrintSelectDialog()
{
}
