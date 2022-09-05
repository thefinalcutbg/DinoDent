#include "DosageDialog.h"
#include "Presenter/DosagePresenter.h"

DosageDialog::DosageDialog(DosagePresenter* p, QWidget* parent) :
	presenter(p), QDialog(parent)
{
	ui.setupUi(this);


	connect(ui.doseFormEdit, &QLineEdit::textChanged, [=](const QString& text) {
		     presenter->formNameChanged(text.toStdString());
		});

	connect(ui.routeEdit, &QLineEdit::textChanged, [=](const QString& text) {
			presenter->routeChanged(text.toStdString());
		});

	connect(ui.doseSpin, &QDoubleSpinBox::valueChanged, [=](double value) {
			presenter->doseQuantityValueChanged(value);
		});

	connect(ui.durationSpin, &QSpinBox::valueChanged, [=](int value) { presenter->durationValueChanged(value); });
	connect(ui.periodSpin, &QSpinBox::valueChanged, [=](int value) { presenter->periodValueChanged(value); });
	connect(ui.frequencySpin, &QSpinBox::valueChanged, [=](int value) { 
		presenter->frequencyChanged(value); 

		ui.frequencyLabel->setText(
			value == 1 ?
			u8" път на "
			:
			u8" пъти на "
		);

		
	});

	ui.doseFormEdit->setInputValidator(&form_validator);
	ui.routeEdit->setInputValidator(&route_validator);

	presenter->setView(this);
}

DosageDialog::~DosageDialog()
{}

void DosageDialog::setDurationList(const std::vector<std::string>&list)
{
	if (!ui.durationCombo->count()) {

		for (auto& s : list) ui.durationCombo->addItem(s.c_str());
		return;
	}

	for (int i = 0; i < ui.durationCombo->count(); i++)
	{
		ui.durationCombo->setItemText(i, list[i].c_str());
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
	ui.doseFormEdit->setCompletions(list);
}

void DosageDialog::setDosageUnit(const std::string& unitName)
{
	QSignalBlocker b(ui.doseFormEdit);

	ui.doseFormEdit->setText(unitName.c_str());
	ui.doseFormEdit->validateInput();
}

void DosageDialog::setDosageLabel(const std::string& label)
{
	ui.formLabel->setText(label.c_str());
}

void DosageDialog::setRouteList(const std::vector<std::string>& list)
{
	ui.routeEdit->setCompletions(list);
}

void DosageDialog::setRouteString(const std::string& route)
{
	QSignalBlocker b(ui.routeEdit);
	ui.routeEdit->setText(route.c_str());
	ui.routeEdit->validateInput();
}
