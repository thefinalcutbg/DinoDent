#include "PrescriptionView.h"
#include "Presenter/PrescriptionPresenter.h"
#include "View/Theme.h"
#include <QPainter>

PrescriptionView::PrescriptionView(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.addButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editButton->setIcon(QIcon(":/icons/icon_edit.png"));
	ui.eRxButton->setIcon(QIcon(":/icons/icon_erx.png"));
	ui.templateButton->setIcon(QIcon(":/icons/icon_template.png"));

	ui.addButton->setHoverColor(Theme::mainBackgroundColor);
	ui.deleteButton->setHoverColor(Theme::mainBackgroundColor);
	ui.editButton->setHoverColor(Theme::mainBackgroundColor);
	ui.eRxButton->setHoverColor(Theme::mainBackgroundColor);
	ui.templateButton->setHoverColor(Theme::mainBackgroundColor);

	ui.medicationTable->setModel(&medModel);

	ui.medicationTable->setMedicationLayot();

	setStyleSheet(Theme::getFancyStylesheet());

    connect(ui.addButton, &QPushButton::clicked, this, [=, this] {if (presenter)presenter->addPressed();});
    connect(ui.editButton, &QPushButton::clicked, this, [=, this] {if (presenter)presenter->editPressed(ui.medicationTable->selectedRow());});
    connect(ui.deleteButton, &QPushButton::clicked, this, [=, this] {

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

    connect(ui.medicationTable, &TableView::deletePressed, this, [=, this](int index) { if (presenter) presenter->deletePressed(index); });
    connect(ui.medicationTable, &TableView::editPressed, this, [=, this](int index) { if (presenter) presenter->editPressed(index); });
	connect(ui.dispensationCombo, &QComboBox::currentIndexChanged, [&] { dispensationLogic(); });
	connect(ui.repeats, &QSpinBox::valueChanged, [&] { if (ui.repeats->isHidden()) return; dispensationLogic(); });
    connect(ui.supplementsEdit, &QLineEdit::textChanged, this, [=, this](const QString& text) {if (presenter) presenter->supplementsChanged(text.toStdString());});
    connect(ui.nrnButton, &QPushButton::clicked, this, [=, this] {if (presenter) presenter->nrnButtonClicked(); });
    connect(ui.dateEdit, &QDateEdit::dateChanged, this, [=, this](QDate d) {if (presenter) presenter->dateChanged(Date{ d.day(),d.month(),d.year() });});
    connect(ui.checkStatusButton, &QPushButton::clicked, this, [=, this]{ if (presenter) presenter->checkStatus(); });
    connect(ui.pregnancyCheck, &QCheckBox::stateChanged, this, [=, this] { sendFemaleProperties(); });
    connect(ui.breastfeedingCheck, &QCheckBox::stateChanged, this, [=, this] { sendFemaleProperties(); });
    connect(ui.eRxButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->eRxPressed(); });
	connect(ui.templateButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->addTemplate(); });
}

IPatientTileInfo* PrescriptionView::patientTile()
{
	return ui.patientInfoTile;
}

void PrescriptionView::setMedicationList(const std::vector<std::string> rows)
{
	medModel.setRows(rows);
	ui.medicationTable->selectRow(rows.size() - 1);
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
			ui.repeats->value()
		});

}

void PrescriptionView::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.fillRect(rect(), Theme::background);
}

void PrescriptionView::sendFemaleProperties()
{
	if (!presenter) return;

	presenter->setFemaleProperties(
		ui.pregnancyCheck->isChecked(),
		ui.breastfeedingCheck->isChecked()
	);
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

void PrescriptionView::setNrn(const std::string& nrn)
{

	bool readOnly = nrn.size();

	ui.addButton->setHidden(readOnly);
	ui.editButton->setDisabled(readOnly);
	ui.deleteButton->setHidden(readOnly);
	ui.editButton->setHidden(readOnly);
	ui.dateEdit->setReadOnly(readOnly);
	ui.dispensationCombo->setDisabled(readOnly);
	ui.repeats->setReadOnly(readOnly);
	ui.supplementsEdit->setReadOnly(readOnly);
	ui.medicationTable->enableContextMenu(!readOnly);
	ui.pregnancyCheck->setDisabled(readOnly);
	ui.breastfeedingCheck->setDisabled(readOnly);
	ui.eRxButton->setHidden(readOnly);
	ui.templateButton->setHidden(readOnly);

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

void PrescriptionView::setMisc(bool isFemale, bool isPregnant, bool isBreastFeeding)
{
	QSignalBlocker p(ui.pregnancyCheck);
	QSignalBlocker b(ui.breastfeedingCheck);

	ui.pregnancyCheck->setChecked(isPregnant);
	ui.breastfeedingCheck->setChecked(isBreastFeeding);

	ui.pregnancyCheck->setHidden(!isFemale);
	ui.breastfeedingCheck->setHidden(!isFemale);
}


PrescriptionView::~PrescriptionView()
{
}
