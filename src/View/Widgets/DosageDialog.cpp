#include "DosageDialog.h"
#include "Presenter/DosagePresenter.h"
#include "View/uiComponents/LineEdit.h"
#include "Model/Prescription/Dosage.h"

DosageDialog::DosageDialog(DosagePresenter& p, QWidget* parent) :
    QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	setWindowTitle("Дозировка");

	ui.offsetCheck->hide();
	ui.offsetSpin->hide();

	ui.addWhenButton->setDisabled(true);

    connect(ui.asNeededCheck, &QCheckBox::stateChanged, this, [&] {
        presenter.setAsNeeded(ui.asNeededCheck->isChecked());
		});

    connect(ui.offsetCheck, &QCheckBox::stateChanged, this, [&] {
			bool enabled = ui.offsetCheck->isChecked();
            presenter.enableOffset(enabled);
			ui.offsetSpin->setEnabled(enabled);
		});

    connect(ui.doseFormCombo->lineEdit(), &QLineEdit::textChanged, this, [&](const QString& text) {
             presenter.formNameChanged(text.toStdString());
		});


    connect(ui.additionalEdit, &QLineEdit::textChanged, this, [&](const QString& text) {
            presenter.additionalInstructionsChanged(text.toStdString());
		});


	connect(ui.doseSpin, &QDoubleSpinBox::valueChanged, this, [&](double value) {
            presenter.doseQuantityValueChanged(value);
		});

    connect(ui.offsetSpin, &QSpinBox::valueChanged, this, [&](int value) {
            presenter.offsetChanged(value);

		});

    connect(ui.periodSpin, &QDoubleSpinBox::valueChanged, this, [&](double value) { presenter.periodValueChanged(value); });
    connect(ui.boundsSpin, &QDoubleSpinBox::valueChanged, this, [&](double value) { presenter.boundsValueChanged(value); });
    connect(ui.frequencySpin,&QSpinBox::valueChanged, this, [&](int value) {
        presenter.frequencyChanged(value);

		/*
		ui.frequencyLabel->setText(
			value == 1 ?
			" път на "
			:
			" пъти на "
		);
		*/
		
	});

    connect(ui.periodCombo, &QComboBox::currentIndexChanged, this, [&](int idx) { presenter.periodUnitChanged(idx);});
    connect(ui.boundsCombo, &QComboBox::currentIndexChanged, this, [&](int idx) { presenter.boundsUnitChanged(idx);});

    connect(ui.okButton, &QPushButton::clicked, this, [&] { presenter.okPressed();});

	LineEdit* routeLine = new LineEdit(ui.routeCombo);
	routeLine->setErrorLabel(ui.errorLabel);
	routeLine->setInputValidator(&route_validator);
	ui.routeCombo->setLineEdit(routeLine);

    connect(ui.routeCombo->lineEdit(), &QLineEdit::textChanged, this, [&](const QString& text) {
		if(static_cast<LineEdit*>(ui.routeCombo->lineEdit())->validateInput())
        presenter.routeChanged(text.toStdString());
	});




    connect(ui.whenCombo->lineEdit(), &QLineEdit::textChanged, this, [&] {
		static_cast<LineEdit*>(ui.whenCombo->lineEdit())->validateInput() ?
			ui.whenCombo->lineEdit()->setStyleSheet("")
			:
			ui.whenCombo->lineEdit()->setStyleSheet("border: 1px solid red");
		});

    connect(ui.addWhenButton, &QPushButton::clicked, this, [&]{

        presenter.whenTagAdded(ui.whenCombo->lineEdit()->text().toStdString());

	});

    connect(ui.whenCombo->lineEdit(), &QLineEdit::returnPressed, this, [&] {

        presenter.whenTagAdded(ui.whenCombo->lineEdit()->text().toStdString());
	});

    whenLine = new LineEdit(ui.whenCombo);
    whenLine->setErrorLabel(ui.errorLabel);
    whenLine->setInputValidator(&when_validator);

    ui.whenCombo->setLineEdit(whenLine);

    connect(whenLine, &QLineEdit::textChanged, this, [&](const QString& text) {
		ui.addWhenButton->setEnabled(whenLine->isValid() && text.size());
	});

    connect(ui.cancelButton, &QPushButton::clicked, this, [&] {close();});

    presenter.setView(this);
}

DosageDialog::~DosageDialog()
{}

void DosageDialog::setBoundsList(const std::vector<std::string>&list)
{
	if (!ui.boundsCombo->count()) {

		for (auto& s : list) ui.boundsCombo->addItem(s.c_str());
		return;
	}

	for (int i = 0; i < ui.boundsCombo->count(); i++)
	{
		ui.boundsCombo->setItemText(i, list[i].c_str());
	}
}

void DosageDialog::setPeriodList(const std::vector<std::string>& list)
{
	
	if (!ui.periodCombo->count()) {

		for (auto& s : list) ui.periodCombo->addItem(s.c_str());
		return;
	}

	for (int i = 0; i < ui.periodCombo->count(); i++)
	{
		ui.periodCombo->setItemText(i, list[i].c_str());
	}
}

void DosageDialog::setDoseFormCompletionList(const std::vector<std::string>& list)
{
	if (ui.doseFormCombo->count()) {

        for (size_t i = 0; i < list.size(); i++) {
			ui.doseFormCombo->setItemText(i, list[i].c_str());
		}

		return;
	}

	for (auto& unit : list) ui.doseFormCombo->addItem(unit.c_str());

}

void DosageDialog::setWhenFormCompletionList(const std::vector<std::string>& list)
{
	for (auto& str : list)
	{
		ui.whenCombo->addItem(str.c_str());
	}
}

void DosageDialog::setDosageUnit(const std::string& unitName)
{
	QSignalBlocker b(ui.doseFormCombo);

	ui.doseFormCombo->setCurrentText(unitName.c_str());

}

void DosageDialog::setRouteList(const std::vector<std::string>& list)
{
	for (auto& item : list)
		ui.routeCombo->addItem(item.c_str());

	ui.routeCombo->lineEdit()->setPlaceholderText("По подразбиране");
	ui.routeCombo->setCurrentIndex(-1);
}

void DosageDialog::setRouteString(const std::string& route)
{
	QSignalBlocker b(ui.routeCombo);
	ui.routeCombo->lineEdit()->setText(route.c_str());
}

void DosageDialog::setWhenTags(const std::vector<std::string>& tags, bool offsetAllowed)
{
	ui.whenCombo->clearEditText();

	while (ui.tagLayout->count())
	{
		ui.tagLayout->takeAt(0)->widget()->deleteLater();
	}

    for (size_t i = 0; i < tags.size(); i++) {

		QString text = tags[i].c_str();
		text.append(" ");

		QPushButton* tagButton = new QPushButton(text, this);

		tagButton->setToolTip("Премахни");

		tagButton->setIcon(QIcon(":/icons/icon_remove.png"));
		tagButton->setLayoutDirection(Qt::RightToLeft);

		ui.tagLayout->addWidget(tagButton);

        QObject::connect(tagButton, &QPushButton::clicked, this, [=, this] {presenter.removeTag(i);});

	}

	ui.offsetSpin->setHidden(!offsetAllowed);
	ui.offsetCheck->setHidden(!offsetAllowed);

	ui.whenCombo->setFocus();
}

bool DosageDialog::fieldsAreValid()
{
	if(!static_cast<LineEdit*>(
		ui.routeCombo->lineEdit())->validateInput()
	)
	{
		ui.routeCombo->setFocus();
		return false;
	}

	if (ui.doseFormCombo->currentText().isEmpty()) {
		ui.errorLabel->setText("Невалидна мерна единица на дозировката");
		return false;
	}

	return true;
}

void DosageDialog::setParsed(const std::string& parsed)
{
	QString parsedText = "Текстов резултат: <b>";
	parsedText.append(parsed.c_str());
	parsedText.append("</b>");

	ui.parseLabel->setText(parsedText);
}


void DosageDialog::setDosage(const Dosage& d)
{
	QSignalBlocker blockers[4]{
		QSignalBlocker{ui.periodCombo},
		QSignalBlocker{ui.boundsCombo},
		QSignalBlocker{ui.periodSpin},
		QSignalBlocker{ui.boundsSpin}
	};

	ui.asNeededCheck->setChecked(d.asNeeded);

	setRouteList(Route::getStringList());
	setPeriodList(d.period.getUnitNamesList());
	setBoundsList(d.bounds.getUnitNamesList());
	setDoseFormCompletionList(d.doseQuantity.unitNames());

	ui.routeCombo->setCurrentText(d.route.getValueStr().c_str());

	ui.doseFormCombo->setCurrentText(d.doseQuantity.getUnitName().c_str());

	ui.doseSpin->setValue(d.doseQuantity.value);
	ui.frequencySpin->setValue(d.frequency);
	ui.periodSpin->setValue(d.period.value);
	ui.boundsSpin->setValue(d.bounds.value);

	ui.periodCombo->setCurrentIndex(d.period.getUnitIndex());

	ui.boundsCombo->setCurrentIndex(d.bounds.getUnitIndex());

	setWhenFormCompletionList(d.when.tagsList());
	setWhenTags(d.when.getTags(), d.when.offsetAllowed());
	
	auto offset = d.when.getOffset();

	ui.offsetCheck->setChecked(offset);
	ui.offsetSpin->setValue(offset);

	ui.additionalEdit->setText(d.additionalInstructions.c_str());



}
