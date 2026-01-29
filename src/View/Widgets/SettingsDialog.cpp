#include "SettingsDialog.h"
#include <QPainter>
#include <QFileDialog>
#include "GlobalSettings.h"
#include "TableViewDialog.h"
#include "View/ModalDialogBuilder.h"
#include "View/Theme.h"

#include <QtGlobal>
#include <QInputDialog>
#include "Model/User.h"
#include "View/Widgets/UnfavourableDialog.h"

SettingsDialog::SettingsDialog(QDialog* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

#ifndef Q_OS_WIN
	ui.winPkcsLabel->hide();
#else
	//ui.tabWidget->setStyleSheet("QTabWidget QStackedWidget {background-color: white;}");
#endif

#ifdef Q_OS_MACOS
	ui.label_11->setMinimumHeight(ui.legalEntityCombo->height() + 2);
    //disable tablet settings
    ui.tabletCombo->hide();
    ui.signSoftButton->hide();
    ui.signSoftEdit->hide();
    ui.label_30->setOpenExternalLinks(true);
    ui.label_30->setText(
        "Интеграцията с таблетите за подписване не е налична "
        "поради липса на предоставени драйвъри за macOS :(<br><br>Можете да помогнете, като направите запитване директно към производителите:<br><br> "
        "<a href=\"https://support.wacom.com/hc/en-us/requests/new\">Поддръжка на таблети Wacom</a><br><br>"
        "<a href=\"https://support.signotec.com/en/support/tickets/new\">Поддръжка на таблети Signotec/Evolis</a>"
        );
    ui.label_34->hide();
    ui.label_31->hide();
#endif
	
	setWindowTitle("Настройки");
	setWindowFlag(Qt::WindowMaximizeButtonHint);
	setWindowIcon(QIcon(":/icons/icon_settings.png"));

	ui.addSubdir->setIcon(QIcon(":/icons/icon_add.png"));
	ui.removeSubdir->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.addDoctor->setIcon(QIcon(":/icons/icon_add.png"));
	ui.removeDoctor->setIcon(QIcon(":/icons/icon_remove.png"));

	ui.printEmptyDocs->setIcon(CommonIcon::getPixmap(CommonIcon::PRINT));
	ui.hisImport->setIcon(CommonIcon::getPixmap(CommonIcon::HIS));
	ui.updateMedButton->setIcon(CommonIcon::getPixmap(CommonIcon::PRESCR));
	ui.sqlTable->setModel(&sql_table_model);
	ui.sqlTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	ui.doctorList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.doctorList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui.doctorList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.doctorList->horizontalHeader()->setHighlightSections(false);

	ui.sqlTable->hide();
	ui.sqlButton->hide();
	ui.sqlEdit->hide();

	ui.tabletErrorLabel->setStyleSheet("color: red;");

	for (auto& specialty : specialties)
	{
		ui.specialtyCombo->addItem(specialty);
	}

	ui.tabletCombo->addItem("Не е посочен");

	for (int i = 1; i < SignatureTablet::s_models.size(); i++)
	{
		ui.tabletCombo->addItem(SignatureTablet::s_models[i].c_str());
	}

	connect(ui.tabletCombo, &QComboBox::currentIndexChanged, this, [&](int idx) {

		ui.signSoftEdit->setText(SignatureTablet::defaultPDFSignerLocation(idx).c_str());
		//ui.signSoftEdit->setDisabled(!idx);
		//ui.signSoftButton->setDisabled(!idx);
		});

	connect(ui.signSoftEdit, &QLineEdit::textChanged, this, [&](const QString& text) {
		
		if (text.isEmpty() || QFileInfo::exists(text)) {
			ui.signSoftEdit->setStyleSheet("");
			return;
		}

		ui.signSoftEdit->setStyleSheet("QLineEdit { color: crimson; }");

	});

	connect(ui.signSoftButton, &QPushButton::clicked, this, [&] {
			
		QString fileName = QFileDialog::getOpenFileName(this, "Изберете програма за подписване на PDF",
			ui.signSoftEdit->text(), //default path here
#ifdef Q_OS_MACOS
			"Application (*.app)"
#else
			"Application (*.exe)"
#endif
		);

		if (fileName.isEmpty()) return;

		ui.signSoftEdit->setText(fileName);

	});

	connect(ui.dirButton, &QPushButton::clicked, this, [&] {

		QString path = QFileDialog::getExistingDirectory(this, "Изберете директория за PDF документите",
			ui.dirEdit->text()
		);

		if (path.isEmpty()) return;

		ui.dirEdit->setText(path);

	});

	connect(ui.addSubdir, &QPushButton::clicked, this, [&] {

		int result = ModalDialogBuilder::openButtonDialog(s_dirStr, "Добавяне на поддиректория", "Изберете поддиректория:");
		if (result < 0) return;

		dir_structure.push_back(static_cast<DirType>(result));

		refreshDirStructureUI();
	});

	connect(ui.removeSubdir, &QPushButton::clicked, this, [&] {

		if (dir_structure.empty()) return;

		dir_structure.pop_back();

		refreshDirStructureUI();
	});


	//practice validators
	ui.practiceNameEdit->setInputValidator(&not_empty_validator);
	ui.rziEdit->setInputValidator(&rzi_validator);
	ui.activityAddressEdit->setInputValidator(&city_validator);
	ui.activityAddressEdit->setCompletions(Ekatte::cityNameToIdx());


	ui.practiceNameEdit->setErrorLabel(ui.errorLabel);
	ui.rziEdit->setErrorLabel(ui.errorLabel);
	ui.activityAddressEdit->setErrorLabel(ui.errorLabel);

	//company validators
	ui.firmAddressEdit->setInputValidator(&not_empty_validator);
	ui.bulstatEdit->setInputValidator(&bulstat_validator);
	ui.ibanEdit->setInputValidator(&iban_validator);
	ui.bicEdit->setInputValidator(&bic_validator);

	connect(ui.legalEntityCombo, &QComboBox::currentIndexChanged, this, [&](int index) {
		legalEntityChanged(index == 0);
		});

	ui.firmAddressEdit->setErrorLabel(ui.errorLabel);
	ui.bulstatEdit->setErrorLabel(ui.errorLabel);
	ui.selfInsuredId->setErrorLabel(ui.errorLabel);
	ui.ibanEdit->setErrorLabel(ui.errorLabel);
	ui.bicEdit->setErrorLabel(ui.errorLabel);

	//nhif validators
	connect(ui.nhifGroup, &QGroupBox::toggled, this, [&](bool clicked) { disableNhifValidators(!clicked); });
	ui.practiceNameNhif->setErrorLabel(ui.errorLabel);
	ui.contractEdit->setErrorLabel(ui.errorLabel);

    connect(ui.unfavButton, &QPushButton::clicked, this, [&]{ UnfavourableDialog().exec(); });

	//doctor validators
	ui.lpkEdit->setInputValidator(&lpk_validator);
	ui.fNameEdit->setInputValidator(&name_validator);
	ui.mNameEdit->setInputValidator(nullptr);
	ui.lNameEdit->setInputValidator(&name_validator);
	ui.phoneEdit->setInputValidator(&not_empty_validator);
	auto phoneValidator = new QRegularExpressionValidator(QRegularExpression("[0-9-+]+"), this);
	ui.phoneEdit->setValidator(phoneValidator);

	ui.lpkEdit->setErrorLabel(ui.errorLabel);
	ui.fNameEdit->setErrorLabel(ui.errorLabel);
	ui.mNameEdit->setErrorLabel(ui.errorLabel);
	ui.lNameEdit->setErrorLabel(ui.errorLabel);
	ui.phoneEdit->setErrorLabel(ui.errorLabel);

	//sms template setters
	ui.smsNotifyTemplate->setTitle("Известие за записано посещние");
	ui.smsReminderTemplate->setTitle("Напомняне за посещение");
	ui.smsCancelTemplate->setTitle("Известие за отказано посещение");

	Settings::SMSSettings smsDefaults;

	ui.smsNotifyTemplate->setDefaultMessageTemplate(smsDefaults.notifTemplate.c_str());
	ui.smsReminderTemplate->setDefaultMessageTemplate(smsDefaults.reminderTemplate.c_str());
	ui.smsCancelTemplate->setDefaultMessageTemplate(smsDefaults.cancelTemplate.c_str());

	connect(ui.specialtyButton, &QPushButton::clicked, this,
		[&] {

			TableViewDialog d(his_specialtyModel, 1, ui.specialtyButton->text().toStdString());
			d.setWindowTitle("Избор на специалност дентална медицина");
			d.exec();

			auto result = d.getResult();

			if (d.getResult().size()) {
				ui.specialtyButton->setText(result.c_str());
				ui.specialtyButton->adjustSize();
			}
		}
	);

	connect(ui.removeDoctor, &QPushButton::clicked, this, [&] { presenter.removeDoctor(); });
	connect(ui.addDoctor, &QPushButton::clicked, this, [&] {presenter.addDoctor(); });
	connect(ui.adminCheck, &QCheckBox::stateChanged, this, [&] {
		presenter.practiceDoctorChanged(ui.specialtyCombo->currentIndex(), ui.adminCheck->isChecked());
		});
	connect(ui.specialtyCombo, &QComboBox::currentIndexChanged, this, [&] {
		presenter.practiceDoctorChanged(ui.specialtyCombo->currentIndex(), ui.adminCheck->isChecked());
		});

	connect(ui.doctorList, &QTableWidget::itemSelectionChanged, this, [=, this]() {

		int row = ui.doctorList->selectedItems().empty() ? -1 : ui.doctorList->selectionModel()->currentIndex().row();

		bool hideDetails = row == -1;

		ui.adminCheck->setHidden(hideDetails);
		ui.specialtyCombo->setHidden(hideDetails);
		ui.label_13->setHidden(hideDetails);
		ui.removeDoctor->setDisabled(hideDetails);

		presenter.doctorIndexChanged(row);

		});

	connect(ui.cancelButton, &QPushButton::clicked, this, [&] {close(); });
	connect(ui.okButton, &QPushButton::clicked, this, [&] {
		//since the validators are members of the view
		//we assume it is view's responsibility to check them
		if (!allFieldsAreValid()) return;
		presenter.okPressed();
		});
	connect(ui.updateMedButton, &QPushButton::clicked, this, [&] {presenter.updateMedications(); });
	connect(ui.addPkcs11, &QPushButton::clicked, this, [&] {

#ifdef Q_OS_WIN
		QString extention = " (*.dll)";
#else

		QString extention = " (*so *dylib)";
#endif

		auto str = QFileDialog::getOpenFileName(
			nullptr,
			"Изберете PKCS11 драйвър",
			"", extention
		);

		if (str.isEmpty()) return;

		ui.pkcs11list->insertItem(0, str.toUtf8());

		});

	connect(ui.removePkcs11, &QPushButton::clicked, this, [&] {

		QList<QListWidgetItem*> items = ui.pkcs11list->selectedItems();

		for (QListWidgetItem* item : items)
		{
			delete ui.pkcs11list->takeItem(ui.pkcs11list->row(item));
		}

		});

	connect(ui.resetDefault, &QPushButton::clicked, this, [&] {

		ui.pkcs11list->clear();

		auto paths = GlobalSettings::getDefaultPkcs11Paths();

		for (auto& p : paths) ui.pkcs11list->addItem(p.c_str());

	});

    connect(ui.tabWidget, &QTabWidget::currentChanged, this, [&](int idx){
        if(idx == static_cast<int>(Tab::Practice)) presenter.practiceTabFocused();
    });

	connect(ui.devBranch, &QCheckBox::clicked, this, [&](bool checked) {

		if (checked) {

			bool answer = ModalDialogBuilder::askDialog(
				"Тестовите актуализации са нестабилни и може да съдържат повече бъгове. "
				"Същевременно чрез тестването и докладването на проблеми вие помагате за развитието на софтуера. "
				"Сигурни ли сте, че искате да включите тази опция? "
			);

			if (!answer) {
				ui.devBranch->setChecked(false);
			}
		}

		});


	connect(ui.sqlButton, &QPushButton::clicked, this, [&] {
		presenter.sqlCommandExec(ui.sqlEdit->text().toStdString());
	});

	connect(ui.sqlAgree, &QPushButton::clicked, this, [&] {
		ui.sqlButton->show();
		ui.sqlEdit->show();
		ui.sqlTable->show();
		ui.okButton->setDefault(false);
		ui.sqlButton->setDefault(true);
		ui.sqlWarning->hide();
		ui.sqlAgree->hide();
		ui.sqlEdit->setFocus();
		
		ui.sql->layout()->removeItem(ui.sqlSpacer1);
		ui.sql->layout()->removeItem(ui.sqlSpacer2);;
		
		delete ui.sqlSpacer1;
		delete ui.sqlSpacer2;

		presenter.sqlCommandExec("SELECT * FROM sqlite_master");

	});

	connect(ui.printEmptyDocs, &QPushButton::clicked, this, [&] { presenter.printEmptyDocs(); });

	connect(ui.hisImport, &QPushButton::clicked, this, [&] { presenter.hisImport(); });

    connect(ui.priceListView, &ProcedureListView::codeDoubleClicked, this, [&](const std::string& code) {
        presenter.priceEditRequested(code);
    });

	connect(ui.sms_balanceButton, &QPushButton::clicked, this, [&] { 
		presenter.balanceRequested(
			ui.smsUsrEdit->text().toStdString(), 
			ui.smsPassEdit->text().toStdString()
		); 
	});

	connect(ui.smsReminderSpin, &QSpinBox::valueChanged, this, [&] {
		ui.smsReminderSpin->setSuffix(ui.smsReminderSpin->value() == 1 ? " час" : " часа");
	});

    connect(ui.printPriceButton, &QPushButton::clicked, this, [&] { presenter.printPriceList(); });

	ui.printPriceButton->setIcon(CommonIcon::getPixmap(CommonIcon::PRINT));

	presenter.setView(this);
	
}

void SettingsDialog::focusTab(Tab tab)
{
	ui.tabWidget->setCurrentIndex(static_cast<int>(tab));
}

void SettingsDialog::setAdminPriv(bool admin)
{
		ui.tabWidget->setTabEnabled(static_cast<int>(Tab::Practice), admin);
		ui.tabWidget->setTabEnabled(static_cast<int>(Tab::Company), admin);
		ui.tabWidget->setTabEnabled(static_cast<int>(Tab::NhifContract), admin);
		ui.tabWidget->setTabEnabled(static_cast<int>(Tab::CodeList), admin);
		ui.tabWidget->setTabEnabled(static_cast<int>(Tab::SQL), admin);
		ui.tabWidget->setTabEnabled(static_cast<int>(Tab::SMS), admin);
		ui.localDbCheck->setDisabled(!admin);
		ui.monthlySheets->setDisabled(!admin);
}

void SettingsDialog::setSettings(const Settings& settings)
{
	ui.autoHisCheck->setChecked(settings.getHisHistoryAuto);
	ui.autoNraCheck->setChecked(settings.getNraStatusAuto);
	ui.autoPisCheck->setChecked(settings.getPisHistoryAuto);
	ui.timeoutSpin->setValue(settings.timeout);
	ui.hirbnoCheck->setChecked(settings.getHirbNoAuto);
	ui.dailyLimitCheck->setChecked(settings.nhifDailyLimitCheck);
	ui.weekendCheck->setChecked(settings.nhifWeekendCheck);
	ui.autoAllergyCheck->setChecked(settings.getAllergiesAuto);
	ui.autoClinicalCheck->setChecked(settings.getClinicalConditionsAuto);
	ui.autoHospitalCheck->setChecked(settings.getHospitalizationAuto);
	ui.showPricesCheck->setChecked(settings.showProcedurePrices);
	ui.monthlySheets->setChecked(settings.preferMonthlySheets);
	ui.smsUsrEdit->setText(settings.sms_settings.usr.c_str());
	ui.smsPassEdit->setText(settings.sms_settings.pass.c_str());
	ui.smsNotifyTemplate->setMessageTemplate(settings.sms_settings.notifTemplate.c_str());
	ui.smsReminderTemplate->setMessageTemplate(settings.sms_settings.reminderTemplate.c_str());
	ui.smsCancelTemplate->setMessageTemplate(settings.sms_settings.cancelTemplate.c_str());
	ui.smsReminderSpin->setValue(settings.sms_settings.reminder_hours);
	ui.smsNotifyTemplate->setAutoCheck(settings.sms_settings.notifAuto);
	ui.smsReminderTemplate->setAutoCheck(settings.sms_settings.reminderAuto);
	ui.smsCancelTemplate->setAutoCheck(settings.sms_settings.cancelAuto);
}

Settings SettingsDialog::getSettings()
{
	return Settings{
		.getHisHistoryAuto = ui.autoHisCheck->isChecked(),
		.getPisHistoryAuto = ui.autoPisCheck->isChecked(),
		.getNraStatusAuto = ui.autoNraCheck->isChecked(),
		.getHirbNoAuto = ui.hirbnoCheck->isChecked(),
		.nhifDailyLimitCheck = ui.dailyLimitCheck->isChecked(),
		.nhifWeekendCheck = ui.weekendCheck->isChecked(),
		.preferMonthlySheets = ui.monthlySheets->isChecked(),
		.getHospitalizationAuto = ui.autoHospitalCheck->isChecked(),
		.getAllergiesAuto = ui.autoAllergyCheck->isChecked(),
		.getClinicalConditionsAuto = ui.autoClinicalCheck->isChecked(),
		.showProcedurePrices = ui.showPricesCheck->isChecked(),
		.timeout = ui.timeoutSpin->value(),
		.sms_settings = {
			.usr = ui.smsUsrEdit->text().toStdString(),
			.pass = ui.smsPassEdit->text().toStdString(),
			.notifTemplate = ui.smsNotifyTemplate->getMessageTemplate().toStdString(),
			.reminderTemplate = ui.smsReminderTemplate->getMessageTemplate().toStdString(),
			.cancelTemplate = ui.smsCancelTemplate->getMessageTemplate().toStdString(),
			.reminder_hours = ui.smsReminderSpin->value(),
			.notifAuto = ui.smsNotifyTemplate->getAutoCheck(),
			.reminderAuto = ui.smsReminderTemplate->getAutoCheck(),
			.cancelAuto = ui.smsCancelTemplate->getAutoCheck()
		}
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

void SettingsDialog::setGlobalSettings(const GlobalSettingsData& data)
{
	ui.pkcs11list->clear();

	for (auto& path : data.pkcs11_list) {
		ui.pkcs11list->addItem(path.c_str());
	}

	QSignalBlocker b1(ui.requestsCheck);
	QSignalBlocker b2(ui.repliesCheck);
	QSignalBlocker b4(ui.devBranch);
    QSignalBlocker b3(ui.localDbCheck);

	ui.requestsCheck->setChecked(data.show_requests);
	ui.repliesCheck->setChecked(data.show_replies);
    ui.localDbCheck->setChecked(data.show_dbDebug);
	ui.devBranch->setChecked(data.dev_branch);
	ui.tabletCombo->setCurrentIndex(data.signer_model);

	if (data.signer_model) {
		ui.signSoftEdit->setText(data.signer_filepath.c_str());
	}

#ifdef Q_OS_MACOS
	ui.tabletCombo->setCurrentIndex(0);
	ui.signSoftEdit->clear();
#endif

	ui.dirEdit->setText(data.pdfDir.c_str());

	dir_structure = data.subdirStructure;

	refreshDirStructureUI();
}

GlobalSettingsData SettingsDialog::getGlobalSettings() {

	GlobalSettingsData data{
		.pkcs11_list = {},
		.dev_branch = ui.devBranch->isChecked(),
		.show_requests = ui.requestsCheck->isChecked(),
		.show_replies = ui.repliesCheck->isChecked(),
        .show_dbDebug = ui.localDbCheck->isChecked(),
		.signer_model = ui.tabletCombo->currentIndex(),
		.signer_filepath = ui.signSoftEdit->text().toStdString(),
		.pdfDir = ui.dirEdit->text().toStdString(),
		.subdirStructure = dir_structure
	};

	for (int i = 0; i < ui.pkcs11list->count(); i++)
		data.pkcs11_list.push_back(ui.pkcs11list->item(i)->text().toUtf8().toStdString());



	return data;
}

void SettingsDialog::disableNhifValidators(bool disabled)
{
	ui.contractEdit->setInputValidator(disabled ? nullptr : &nhif_contract_validator);
	ui.practiceNameNhif->setInputValidator(disabled ? nullptr : &not_empty_validator);

	ui.contractEdit->validateInput();
	ui.practiceNameEdit->validateInput();

	ui.monthlySheets->setHidden(!disabled);
}

void SettingsDialog::legalEntityChanged(bool selfInsured)
{
	ui.selfInsuredId->setInputValidator(selfInsured ? &grao_validator : nullptr);
	ui.mNameEdit->setInputValidator(selfInsured ? &name_validator : nullptr);
	ui.selfInsuredId->setHidden(!selfInsured);
    ui.selfInsuredLabel->setHidden(!selfInsured);
}

QString SettingsDialog::getDoctorName(const PracticeDoctor &entity)
{
	QString result = "д-р ";

	result += entity.lpk != User::doctor().LPK ?
		QString::fromStdString(entity.name)
		:
		ui.fNameEdit->text() + " " + ui.lNameEdit->text();

	return result;
}

ProcedureListView* SettingsDialog::getPriceListView()
{
	return ui.priceListView;
}

void SettingsDialog::setSqlTable(const PlainTable& table)
{
	sql_table_model.setTableData(table);
	ui.sqlEdit->clear();
	ui.sqlEdit->setFocus();
}

void SettingsDialog::refreshDirStructureUI()
{
	QString text;

	for (auto value : dir_structure) {
		text += s_dirStr[value];
		text += "->";
	}

	if (text.size()) {
		text.removeLast();
		text.removeLast();
	}

	ui.subDirEdit->setText(text);
}

void SettingsDialog::setPractice(const Practice& practice)
{
	ui.practiceNameEdit->setText(QString::fromStdString(practice.name));
	ui.rziEdit->setText(QString::fromStdString(practice.rziCode));
	ui.bulstatEdit->setText(QString::fromStdString(practice.bulstat));
	ui.legalEntityCombo->setCurrentIndex(practice.legal_entity);
	ui.firmAddressEdit->setText(QString::fromStdString(practice.firm_address));
	ui.activityAddressEdit->setText(practice.practice_address.getString(false).c_str());
	ui.activityStreetEdit->setText(practice.street_address.c_str());
	ui.vatCheck->setChecked(practice.hasVat);
	ui.practicePassEdit->setText(QString::fromStdString(practice.pass));
	ui.selfInsuredId->setText(QString::fromStdString(practice.selfInsuredId));
	ui.bankEdit->setText(practice.bank.c_str());
	ui.ibanEdit->setText(practice.iban.c_str());
	ui.bicEdit->setText(practice.bic.c_str());


	ui.practicePassEdit->setEchoMode(
		practice.pass.empty() ? QLineEdit::Normal : QLineEdit::Password
	);

	if (practice.nhif_contract) {
		ui.contractEdit->setText(QString::fromStdString(practice.nhif_contract.value().contract_no));
		ui.contractDateEdit->set_Date(practice.nhif_contract.value().date);
		ui.practiceNameNhif->setText(QString::fromStdString(practice.nhif_contract.value().name_short));
		ui.nraPass->setText(practice.nhif_contract->nra_pass.data());
		ui.nssiPass->setText(practice.nhif_contract->nssi_pass.c_str());
		ui.iamnEdit->setText(practice.nhif_contract->dentalTechnicianCode.c_str());
	}

	ui.nhifGroup->setChecked(practice.nhif_contract.has_value());
}

void SettingsDialog::setDoctor(const Doctor& doctor)
{
	ui.doctorPassEdit->setEchoMode(QLineEdit::EchoMode::Password);

	ui.lpkEdit->set_Text(doctor.LPK);
	ui.fNameEdit->set_Text(doctor.fname);
	ui.mNameEdit->set_Text(doctor.mname);
	ui.lNameEdit->set_Text(doctor.lname);
	ui.phoneEdit->set_Text(doctor.phone);
	ui.doctorPassEdit->set_Text(doctor.pass);

	ui.doctorPassEdit->setEchoMode(doctor.pass.empty() ? 
		QLineEdit::Normal
		:
		QLineEdit::Password
	);

	auto specialtyName = doctor.hisSpecialty.getName();

	ui.specialtyButton->setText(
		specialtyName.empty() ? "Изберете специалност" : specialtyName.c_str()
	);

	ui.specialtyButton->adjustSize();
}

void SettingsDialog::setDoctorList(const std::vector<PracticeDoctor>& doctors)
{
	ui.doctorList->clearContents();

	bool nhif = ui.nhifGroup->isChecked();

	ui.doctorList->setRowCount(doctors.size());

	for (int i = 0; i < doctors.size(); i++) {
		auto& doc = doctors[i];
		
		ui.doctorList->setItem(i, 0, new QTableWidgetItem(getDoctorName(doc)));
		ui.doctorList->setItem(i, 1, new QTableWidgetItem(specialties[static_cast<int>(doc.specialty)]));
		ui.doctorList->setItem(i, 2, new QTableWidgetItem(doc.admin ? "ДА" : "НЕ"));
		ui.doctorList->item(i, 2)->setTextAlignment(Qt::AlignCenter);
	}

	ui.adminCheck->setHidden(true);
	ui.specialtyCombo->setHidden(true);
	ui.label_13->setHidden(true);
	ui.removeDoctor->setDisabled(true);
}

void SettingsDialog::setDoctorProperties(bool admin, NhifSpecialty specialty)
{
	//recursion guards
	QSignalBlocker blocker(ui.adminCheck);
	QSignalBlocker blocker2(ui.specialtyCombo);

	ui.adminCheck->setChecked(admin);
	ui.specialtyCombo->setCurrentIndex(static_cast<int>(specialty));
}

Practice SettingsDialog::getPractice()
{
	Practice p;

	p.rziCode = ui.rziEdit->getText();
	p.bulstat = ui.bulstatEdit->getText();
	p.firm_address = ui.firmAddressEdit->getText();
	p.practice_address = ui.activityAddressEdit->getText();
	p.street_address = ui.activityStreetEdit->getText();
	p.legal_entity = ui.legalEntityCombo->currentIndex();
	p.name = ui.practiceNameEdit->getText();
	p.pass = ui.practicePassEdit->getText();
	p.hasVat = ui.vatCheck->isChecked();
	p.selfInsuredId = ui.selfInsuredId->getText();
	p.bank = ui.bankEdit->getText();
	p.iban = ui.ibanEdit->getText();
	p.bic = ui.bicEdit->getText();

	if (ui.nhifGroup->isChecked())
	{
		NhifContract c;
		c.date = ui.contractDateEdit->getDate();
		c.contract_no = ui.contractEdit->getText();
		c.name_short = ui.practiceNameNhif->getText();
		c.dentalTechnicianCode = ui.iamnEdit->getText();
		c.nra_pass = ui.nraPass->getText();
		c.nssi_pass = ui.nssiPass->getText();
		p.nhif_contract.emplace(c);
	}

	return p;
}

Doctor SettingsDialog::getDoctor()
{
	Doctor doctor;
	doctor.LPK = ui.lpkEdit->getText();
	doctor.fname = ui.fNameEdit->getText();
	doctor.mname = ui.mNameEdit->getText();
	doctor.lname = ui.lNameEdit->getText();
	doctor.phone = ui.phoneEdit->getText();
	doctor.hisSpecialty = ui.specialtyButton->text().toStdString();
	doctor.pass = ui.doctorPassEdit->getText();

	return doctor;
}


void SettingsDialog::replaceCurrentItem(const PracticeDoctor& item)
{
    ui.doctorList->item(ui.doctorList->currentRow(), 0)->setText(getDoctorName(item));
	ui.doctorList->item(ui.doctorList->currentRow(), 1)->setText(specialties[static_cast<int>(item.specialty)]);
	ui.doctorList->item(ui.doctorList->currentRow(), 2)->setText(item.admin ? "ДА" : "НЕ");
}

bool SettingsDialog::allFieldsAreValid()
{
	constexpr int fieldsSize = 15;

	AbstractLineEdit* fields[fieldsSize] {
		ui.practiceNameEdit,
		ui.activityAddressEdit,
		ui.rziEdit,
		ui.selfInsuredId,
		ui.firmAddressEdit,
		ui.bulstatEdit,
		ui.ibanEdit,
		ui.bicEdit,
		ui.lpkEdit,
		ui.fNameEdit,
		ui.mNameEdit,
		ui.lNameEdit,
		ui.phoneEdit,
		ui.contractEdit,
		ui.practiceNameNhif
	};


	for (int i = 0; i < fieldsSize; i ++) {

		auto field = fields[i];

		if(!field->validateInput()) {

			if (i < 3) ui.tabWidget->setCurrentWidget(ui.practiceSettings);
			else if (i < 8) ui.tabWidget->setCurrentWidget(ui.companySettings);
			else if (i < 13) ui.tabWidget->setCurrentWidget(ui.doctorSettings);
			else ui.tabWidget->setCurrentWidget(ui.nhifSettings);

			field->set_focus();	
			return false;
		}
	}

	return true;
}


SettingsDialog::~SettingsDialog()
{
}
