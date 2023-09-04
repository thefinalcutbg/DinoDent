#include "PrescriptionView.h"
#include "Presenter/PrescriptionPresenter.h"
#include "View/Theme.h"
#include "QtVersion.h"
#include <QPainter>

PrescriptionView::PrescriptionView(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.addButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editButton->setIcon(QIcon(":/icons/icon_edit.png"));

	ui.medicationTable->setModel(&medModel);

	ui.medicationTable->setMedicationLayot();

	setStyleSheet(Theme::getFancyStylesheet());

	connect(ui.addButton, &QPushButton::clicked, [=] {if (presenter)presenter->addPressed();});
	connect(ui.editButton, &QPushButton::clicked, [=] {if (presenter)presenter->editPressed(ui.medicationTable->selectedRow());});
	connect(ui.deleteButton, &QPushButton::clicked, [=] {

		if (!presenter) return;

		int currentIdx = ui.medicationTable->selectedRow();
		int lastIdx = ui.medicationTable->verticalHeader()->count() - 1;

		presenter->deletePressed(currentIdx);

		if (currentIdx == lastIdx)
		{
			ui.medicationTable->selectRow(currentIdx - 1);
		}
		else ui.medicationTable->selectRow(currentIdx);

		});


	connect(ui.medicationTable, &TableView::deletePressed, [=](int index) { if (presenter) presenter->deletePressed(index); });
	connect(ui.medicationTable, &TableView::editPressed, [=](int index) { if (presenter) presenter->editPressed(index); });
	connect(ui.dispensationCombo, QtComboIndexChanged, [&] { dispensationLogic(); });
	connect(ui.repeats, QtSpinValueChanged, [&] { if (ui.repeats->isHidden()) return; dispensationLogic(); });
	connect(ui.supplementsEdit, &QLineEdit::textChanged, [=](const QString& text) {if (presenter) presenter->supplementsChanged(text.toStdString());});
	connect(ui.nrnButton, &QPushButton::clicked, [=] {if (presenter) presenter->nrnButtonClicked(); });
	connect(ui.dateEdit, &QDateEdit::dateChanged, [=](QDate d) {if (presenter) presenter->dateChanged(Date{ d.day(),d.month(),d.year() });});
	connect(ui.checkStatusButton, &QPushButton::clicked, [=]{ if (presenter) presenter->checkStatus(); });
}

IPatientTileInfo* PrescriptionView::patientTile()
{
	return ui.patientInfoTile;
}

void PrescriptionView::setMedicationList(const std::vector<std::string> rows)
{
	medModel.setRows(rows);
}

void PrescriptionView::dispensationLogic()
{
	if (!presenter) return;

	auto dispensationIdx = ui.dispensationCombo->currentIndex();

	switch (dispensationIdx)
	{
		case 0:
		{
			ui.repeats->hide();
			ui.repeatsLabel->hide();
			ui.repeats->setValue(1);
			break;
		}
		case 1:
		{
			ui.repeats->show();
			int repeats = ui.repeats->value();
			ui.repeatsLabel->show();
			break;
		}
		case 2:
		{
			ui.repeats->hide();
			ui.repeatsLabel->hide();
			ui.repeats->setValue(1);
			break;
		}
	}

	presenter->dispensationChanged(
		Dispensation{ 
			static_cast<Dispensation::Type>(dispensationIdx),
			static_cast<unsigned int>(ui.repeats->value()) 
		});

}

void PrescriptionView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.fillRect(rect(), Theme::background);
}

void PrescriptionView::setDispensation(const Dispensation& d)
{
	QSignalBlocker b1(ui.repeats);
	QSignalBlocker b2(ui.dispensationCombo);

	ui.dispensationCombo->setCurrentIndex(static_cast<int>(d.type));
	ui.repeats->setValue(d.repeats);
	
	ui.repeats->setHidden(d.type != Dispensation::Type::MultipleUse);
	ui.repeatsLabel->setHidden(d.type != Dispensation::Type::MultipleUse);
}

void PrescriptionView::setSupplements(const std::string& supplements)
{
	QSignalBlocker b(ui.supplementsEdit);
	ui.supplementsEdit->setText(supplements.c_str());
}

void PrescriptionView::setDate(const Date& d)
{
	QSignalBlocker b(ui.dateEdit);
	ui.dateEdit->setDate(QDate(d.year, d.month, d.day));
}

void PrescriptionView::setReadOnly(bool readOnly)
{
	ui.addButton->setHidden(readOnly);
	ui.editButton->setDisabled(readOnly);
	ui.deleteButton->setHidden(readOnly);
	ui.editButton->setHidden(readOnly);
	ui.dateEdit->setReadOnly(readOnly);
	ui.dispensationCombo->setDisabled(readOnly);
	ui.repeats->setReadOnly(readOnly);
	ui.supplementsEdit->setReadOnly(readOnly);
	ui.medicationTable->enableContextMenu(!readOnly);

}

void PrescriptionView::setNrn(const std::string& nrn)
{
	if (nrn.empty()) {

		ui.checkStatusButton->hide();
		ui.nrnButton->setText("Изпрати към НЗИС");
		ui.nrnButton->setHoverText("Изпрати към НЗИС");
		ui.nrnLabel->setText("");
		return;
	}

	ui.checkStatusButton->show();
	ui.nrnLabel->setText("НРН:");
	ui.nrnButton->setText(nrn.c_str());
	ui.nrnButton->setHoverText("Анулирай");

}


PrescriptionView::~PrescriptionView()
{
}