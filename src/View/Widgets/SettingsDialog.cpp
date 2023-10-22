#include "SettingsDialog.h"
#include <QPainter>
#include <QFileDialog>
#include "GlobalSettings.h"

SettingsDialog::SettingsDialog(QDialog*parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Настройки");
	setWindowIcon(QIcon(":/icons/icon_settings.png"));
	ui.generalSettings->setStyleSheet("QWidget#generalSettings{background-color: white;}");
	//ui.tabWidget->tabBar()->setStyleSheet("QTabBar::tab:selected{background-color: white;}");
	ui.tabWidget->setStyleSheet(
		"QTabBar::tab:selected {background-color: white}"
	);
	connect(ui.cancelButton, &QPushButton::clicked, [&] {close();});
	connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed();});
	connect(ui.updateMedButton, &QPushButton::clicked, [&] {presenter.updateMedications();});
	connect(ui.addPkcs11, &QPushButton::clicked, [&] {
		
		auto str = QFileDialog::getOpenFileName(
			nullptr,
			"Изберете PKCS11 драйвър",
			"", " (*.dll)"
		);

		if (str.isEmpty()) return;

		ui.pkcs11list->addItem(str.toUtf8());

	});

	connect(ui.removePkcs11, &QPushButton::clicked, [&] {

		QList<QListWidgetItem*> items = ui.pkcs11list->selectedItems();

		for(QListWidgetItem* item : items)
		{
			delete ui.pkcs11list->takeItem(ui.pkcs11list->row(item));
		}

	});

	connect(ui.resetDefault, &QPushButton::clicked, [&]{

		ui.pkcs11list->clear();

		auto paths = GlobalSettings::getDefaultPkcs11Paths();

		for (auto& p : paths) ui.pkcs11list->addItem(p.c_str());

	});


	ui.practiceSettings->hidePassword();

	presenter.setView(this);
	
}


void SettingsDialog::setSettings(const Settings& settings)
{
	ui.autoHisCheck->setChecked(settings.getHisHistoryAuto);
	ui.autoNraCheck->setChecked(settings.getNraStatusAuto);
	ui.autoPisCheck->setChecked(settings.getPisHistoryAuto);
	ui.timeoutSpin->setValue(settings.timeout);
}

Settings SettingsDialog::getSettings()
{
	return Settings{
		.getHisHistoryAuto = ui.autoHisCheck->isChecked(),
		.getPisHistoryAuto = ui.autoPisCheck->isChecked(),
		.getNraStatusAuto = ui.autoNraCheck->isChecked(),
		.timeout = ui.timeoutSpin->value()
	};
}

void SettingsDialog::setUpdateDate(DynamicNum num, const Date& date)
{
	QString dateStr = date.isDefault() ? "Няма" : date.toBgStandard(true).c_str();

	switch (num)
	{
	case DynamicNum::Medication:
		ui.medUpdateLabel->setText(dateStr);
		break;
	}
}

void SettingsDialog::setPkcs11List(const std::vector<std::string>& list)
{
	ui.pkcs11list->clear();

	for (auto& path : list) ui.pkcs11list->addItem(path.c_str());
}

std::vector<std::string> SettingsDialog::getPkcs11List()
{
	std::vector<std::string> result;

	for (int i = 0; i < ui.pkcs11list->count(); i++)
		result.push_back(ui.pkcs11list->item(i)->text().toUtf8().toStdString());

	return result;
}

void SettingsDialog::setDebug(bool showRequests, bool showReplies)
{

	QSignalBlocker b1(ui.requestsCheck);
	QSignalBlocker b2(ui.repliesCheck);

	ui.requestsCheck->setChecked(showRequests);
	ui.repliesCheck->setChecked(showReplies);
}

bool SettingsDialog::showRequests()
{
	return ui.requestsCheck->isChecked();
}

bool SettingsDialog::showReplies()
{
	return ui.repliesCheck->isChecked();
}

void SettingsDialog::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event); return;
	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);
}

SettingsDialog::~SettingsDialog()
{
}
