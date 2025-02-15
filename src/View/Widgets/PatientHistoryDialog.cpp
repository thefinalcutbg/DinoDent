﻿#include "PatientHistoryDialog.h"
#include "Presenter/PatientHistoryPresenter.h"
#include "View/GlobalFunctions.h"
#include <set>
#include <array>

PatientHistoryDialog::PatientHistoryDialog(PatientHistoryPresenter& p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Онлайн пациентско досие");
	setWindowIcon(QIcon(":/icons/icon_history.png"));

	ui.procedureTable->setModel(&procedure_model);
	ui.docView->setModel(&doc_model);
	ui.docDetailsView->setModel(&doc_details_model);
	ui.hospiTable->setModel(&hospi_model);

	ui.pisProcRadio->setIcon(CommonIcon::getPixmap(CommonIcon::NHIF));
	ui.hisProcRadio->setIcon(CommonIcon::getPixmap(CommonIcon::HIS));
	ui.localProcRadio->setIcon(CommonIcon::getPixmap(CommonIcon::DATABASE));

	ui.hisStatRadio->setIcon(CommonIcon::getPixmap(CommonIcon::HIS));
	ui.localStatRadio->setIcon(CommonIcon::getPixmap(CommonIcon::DATABASE));

	ui.refreshProcedures->setIcon(CommonIcon::getPixmap(CommonIcon::REFRESH));
	ui.refreshStatus->setIcon(CommonIcon::getPixmap(CommonIcon::REFRESH));
	ui.refreshHospi->setIcon(CommonIcon::getPixmap(CommonIcon::REFRESH));
	//init procedure table
	ui.procedureTable->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

	ui.procedureTable->hideColumn(0);
	ui.procedureTable->hideColumn(7);

	ui.procedureTable->setColumnWidth(1, 110);
	ui.procedureTable->setColumnWidth(2, 200);
	ui.procedureTable->setColumnWidth(3, 65);
	ui.procedureTable->setColumnWidth(4, 200);
	ui.procedureTable->setColumnWidth(5, 65);
	ui.procedureTable->setColumnWidth(6, 100);
	ui.procedureTable->setColumnWidth(8, 200);
	
	setTableViewDefaults(ui.procedureTable);
	setTableViewDefaults(ui.hospiTable);
	setTableViewDefaults(ui.docDetailsView);
	setTableViewDefaults(ui.docView);
	ui.docView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

	//hospitalization
	ui.hospiTable->setModel(&hospi_model);

	ui.hospiTable->setColumnWidth(0, 120);
	ui.hospiTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	

	connect(ui.docView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&](const QItemSelection&, const QItemSelection&) {

		auto idx_rows = ui.docView->selectionModel()->selectedRows();

		std::set<int> rows;

		for (auto& idx : idx_rows) {
			rows.insert(idx.row());
		}

		if (rows.size() != 1) {
			doc_details_model.setTableData({});
			return;
		}

		auto currentRow = *rows.begin();

		doc_details_model.setTableData(details_data[currentRow]);

		for (int i = 0; i < details_data[currentRow].size(); i++) {

			ui.docDetailsView->setColumnHidden(i, details_data[currentRow][i].hidden);

			ui.docDetailsView->setColumnWidth(i, details_data[currentRow][i].width);
		}
	});

	connect(ui.openDocButton, &QPushButton::clicked, this, [&] {


		auto idx_rows = ui.docView->selectionModel()->selectedRows();

		std::set<int> rows;

		for (auto& idx : idx_rows) {
			rows.insert(idx.row());
		}

		std::vector<int> docsToOpen;

		for (auto& row : rows) {
			docsToOpen.push_back(row);
		}

		presenter.openDocuments(docsToOpen);

	});

	connect(ui.docView, &QTableView::doubleClicked, this, [&] { ui.openDocButton->click(); });

	connect(ui.applyPISProcedures, &QPushButton::clicked, this, [&] {
		presenter.applyPISprocedures();
	});

	connect(ui.applyToStatus, &QPushButton::clicked, this, [&] {

		auto snapshotPtr = ui.snapshotViewer->getCurrentSnapshot();

		if (!snapshotPtr) return;

		presenter.applyStatus(snapshotPtr->teeth);
	});

	connect(ui.pisProcRadio, &QRadioButton::clicked, this, [&] {
		ui.applyPISProcedures->show();
		ui.refreshProcedures->setDisabled(false);
		ui.procedureTable->setColumnHidden(7, true); 
		presenter.procedureSourceChanged(Procedure::DatabaseSource::PIS); 
	});

	connect(ui.hisProcRadio, &QRadioButton::clicked, this, [&]{
		ui.applyPISProcedures->hide();
		ui.refreshProcedures->setDisabled(false);
		ui.procedureTable->setColumnHidden(7, true); 
		presenter.procedureSourceChanged(Procedure::DatabaseSource::HIS); 
	});
	connect(ui.localProcRadio, &QRadioButton::clicked, this, [&]{
		ui.applyPISProcedures->hide();
		ui.refreshProcedures->setDisabled(true);
		ui.procedureTable->setColumnHidden(7, false); 
		presenter.procedureSourceChanged(Procedure::DatabaseSource::Local); 
	});

	connect(ui.hisStatRadio, &QRadioButton::clicked, this, [&] { 
		ui.snapshotViewer->setSnapshots({});
		ui.refreshStatus->setDisabled(false);
		presenter.statusSourceChanged(Procedure::DatabaseSource::HIS); 
	});
	connect(ui.localStatRadio, &QRadioButton::clicked, this, [&] { 
		ui.snapshotViewer->setSnapshots({});
		ui.refreshStatus->setDisabled(true);
		presenter.statusSourceChanged(Procedure::DatabaseSource::Local); 
	});

	connect(ui.refreshProcedures, &QPushButton::clicked, this, [&] { presenter.procedureRefreshRequested(getProcedureSrc()); });
	connect(ui.refreshStatus, &QPushButton::clicked, this, [&] { if (!ui.hisStatRadio->isChecked()) return; presenter.statusRefreshRequested(); });
	connect(ui.refreshHospi, &QPushButton::clicked, this, [&] { if (!ui.hisStatRadio->isChecked()) return; presenter.hospitalizationRequested(); });

	connect(ui.tabWidget, &QTabWidget::currentChanged, this, [&](int idx) { tabChanged(idx);});
}

void PatientHistoryDialog::setProcedures(const std::vector<Procedure> procedures, Procedure::DatabaseSource source)
{
	std::map<Procedure::DatabaseSource, QRadioButton*> radioSrcMap{
		{ Procedure::PIS, ui.pisProcRadio, },
		{ Procedure::HIS, ui.hisProcRadio },
		{ Procedure::Local, ui.localProcRadio }
	};

	if (!radioSrcMap[source]->isChecked()) return;

	procedure_model.setProcedures(procedures);
}

void PatientHistoryDialog::setDocuments(const PlainTable& docList, const std::vector<PlainTable>& contents)
{
	doc_model.setTableData(docList);

	for (int i = 0; i < docList.size(); i++) {

		ui.docView->setColumnHidden(i, docList[i].hidden);

		ui.docView->setColumnWidth(i, docList[i].width);
	}

	details_data = contents;

}

void PatientHistoryDialog::setSnapshots(const std::vector<HisSnapshot>& snapshots, Procedure::DatabaseSource source)
{
	Procedure::DatabaseSource current =
		ui.localStatRadio->isChecked() ?
		Procedure::Local
		:
		Procedure::HIS
	;

	if (current != source) return;

	ui.snapshotViewer->setSnapshots(snapshots);
}

void PatientHistoryDialog::setHospitalizations(const std::vector<Hospitalization>& h)
{
	hospi_model.setRows(h);
}

void PatientHistoryDialog::open(bool nhif)
{
	if (nhif) {

		QSignalBlocker b1(ui.tabWidget);
		QSignalBlocker b2(ui.pisProcRadio);

		ui.tabWidget->setCurrentIndex(0);

		ui.pisProcRadio->click();
	}
	else {

		QSignalBlocker b1(ui.tabWidget);
		QSignalBlocker b2(ui.hisProcRadio);

		ui.tabWidget->setCurrentIndex(2);

		ui.hisProcRadio->click();

		ui.pisProcRadio->hide();
	}

	emit ui.tabWidget->currentChanged(nhif ? 0 : 1);

	exec();
}

void PatientHistoryDialog::tabChanged(int idx)
{
	switch (idx) {
	case 0:
		presenter.procedureSourceChanged(getProcedureSrc());
		break;
	case 1:
		presenter.statusSourceChanged(
			ui.hisStatRadio->isChecked() ?
			Procedure::HIS : Procedure::Local
		);
		break;
	case 3:
		presenter.hospitalizationRequested();
	}
}

Procedure::DatabaseSource PatientHistoryDialog::getProcedureSrc()
{
	std::vector<std::pair<Procedure::DatabaseSource, QRadioButton*>> pair{
		{ Procedure::PIS, ui.pisProcRadio },
		{ Procedure::HIS, ui.hisProcRadio },
		{ Procedure::Local, ui.localProcRadio }
	};

	for (auto& [src, button] : pair) {
		if (button->isChecked()) return src;
	}

	QSignalBlocker b(ui.localProcRadio);
	ui.localProcRadio->click();

	return Procedure::Local;
}

PatientHistoryDialog::~PatientHistoryDialog()
{}
