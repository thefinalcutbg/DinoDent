#include "SettingsDialog.h"
#include <QPainter>
#include <QFileDialog>
#include "GlobalSettings.h"
#include "TableViewDialog.h"
#include "View/ModalDialogBuilder.h"
#include <QPainter>
#include <QtGlobal>
#include <QInputDialog>
#include "Model/User.h"


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

#endif

	setWindowTitle("Настройки");
	setWindowFlag(Qt::WindowMaximizeButtonHint);
	setWindowIcon(QIcon(":/icons/icon_settings.png"));

	ui.addSubdir->setIcon(QIcon(":/icons/icon_add.png"));
	ui.removeSubdir->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.printEmptyDocs->setIcon(CommonIcon::getPixmap(CommonIcon::PRINT));
	ui.hisImport->setIcon(CommonIcon::getPixmap(CommonIcon::HIS));
	ui.updateMedButton->setIcon(CommonIcon::getPixmap(CommonIcon::PRESCR));
	ui.sqlTable->setModel(&sql_table_model);
	ui.sqlTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	ui.sqlTable->hide();
	ui.sqlButton->hide();
	ui.sqlEdit->hide();

	constexpr int specIdxSize = 5;

	constexpr const char* specialties[specIdxSize]{
		"Няма договор с НЗОК",
		"64 - Общопрактикуващ лекар по дентална медицина",
		"61 - Детски лекар по дентална медицина",
		"62 - Орална хирургия",
		"68 - Дентална, орална и лицево-челюстна хирургия"
	};

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

		int result = ModalDialogBuilder::openButtonDialog(s_dirStr, "Добавяне на поддиректория", "Избере поддиректория:");
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

	//doctor validators
	ui.lpkEdit->setInputValidator(&lpk_validator);
	ui.fNameEdit->setInputValidator(&name_validator);
	ui.mNameEdit->setInputValidator(&name_validator);
	ui.lNameEdit->setInputValidator(&name_validator);
	ui.phoneEdit->setInputValidator(&not_empty_validator);
	auto phoneValidator = new QRegularExpressionValidator(QRegularExpression("[0-9-+]+"), this);
	ui.phoneEdit->setValidator(phoneValidator);

	ui.lpkEdit->setErrorLabel(ui.errorLabel);
	ui.fNameEdit->setErrorLabel(ui.errorLabel);
	ui.mNameEdit->setErrorLabel(ui.errorLabel);
	ui.lNameEdit->setErrorLabel(ui.errorLabel);
	ui.phoneEdit->setErrorLabel(ui.errorLabel);

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

	connect(ui.doctorList, &QListWidget::itemSelectionChanged, this, [=, this]() {

		int row = ui.doctorList->selectedItems().empty() ? -1 : ui.doctorList->selectionModel()->currentIndex().row();

		bool hideDetails = row == -1;

		ui.adminCheck->setHidden(hideDetails);
		ui.specialtyCombo->setHidden(hideDetails);
		ui.label_13->setHidden(hideDetails);
		ui.removeDoctor->setHidden(hideDetails);

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
        if(idx == static_cast<int>(SettingsTab::Practice)) presenter.practiceTabFocused();
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

	connect(ui.priceListView, &ProcedureListView::codeDoubleClicked, this, [&](const std::string& code, bool nhif, double price) {
		/*
		if (code.length() < 5) return; //not ACHI, but just a bloack

		bool ok;
		double newPrice = QInputDialog::getDouble(this, "Въведете цена", "Цена", price, 0, 10000, 2, &ok);

		if (ok) {
			presenter.priceUpdated(code, newPrice);
		}
		*/
	});

	presenter.setView(this);
	
}

void SettingsDialog::focusTab(SettingsTab tab)
{
	ui.tabWidget->setCurrentIndex(static_cast<int>(tab));
}

void SettingsDialog::disableTab(SettingsTab tab)
{
	ui.tabWidget->setTabEnabled(static_cast<int>(tab), false);
}


void SettingsDialog::setSettings(const Settings& settings)
{
	ui.autoHisCheck->setChecked(settings.getHisHistoryAuto);
	ui.autoNraCheck->setChecked(settings.getNraStatusAuto);
	ui.autoPisCheck->setChecked(settings.getPisHistoryAuto);
	ui.autoStatus->setChecked(settings.autoStatus);
	ui.timeoutSpin->setValue(settings.timeout);
	ui.hirbnoCheck->setChecked(settings.getHirbNoAuto);
	ui.dailyLimitCheck->setChecked(settings.nhifDailyLimitCheck);
	ui.weekendCheck->setChecked(settings.nhifWeekendCheck);
	ui.patientList->setChecked(settings.showPatientList);
	ui.autoDiagnosis->setChecked(settings.autoDiagnosis);
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
		.autoStatus = ui.autoStatus->isChecked(),
		.autoDiagnosis = ui.autoDiagnosis->isChecked(),
		.showPatientList = ui.patientList->isChecked(),
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

void SettingsDialog::setGlobalSettings(const GlobalSettingsData& data)
{
	ui.pkcs11list->clear();

	for (auto& path : data.pkcs11_list) {
		ui.pkcs11list->addItem(path.c_str());
	}

	QSignalBlocker b1(ui.requestsCheck);
	QSignalBlocker b2(ui.repliesCheck);
	QSignalBlocker b4(ui.devBranch);

	ui.requestsCheck->setChecked(data.show_requests);
	ui.repliesCheck->setChecked(data.show_replies);
	ui.devBranch->setChecked(data.dev_branch);
	ui.tabletCombo->setCurrentIndex(data.signer_model);

	if (data.signer_model) {
		ui.signSoftEdit->setText(data.signer_filepath.c_str());
	}

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
}

void SettingsDialog::legalEntityChanged(bool selfInsured)
{
	ui.selfInsuredId->setInputValidator(selfInsured ? &grao_validator : nullptr);

	ui.selfInsuredId->setHidden(!selfInsured);
    ui.selfInsuredLabel->setHidden(!selfInsured);
}

QString SettingsDialog::getDoctorName(const PracticeDoctor &entity)
{
    QString suffix = entity.admin ? " (администратор)" : "";

    if (entity.lpk != User::doctor().LPK) {
        return QString::fromStdString(entity.name) + suffix;
    }

    return ui.fNameEdit->text() + " " + ui.lNameEdit->text() + suffix;
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
		ui.unfavCheck->setChecked(practice.nhif_contract->unfavourable);
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
	ui.severalRHIFcheck->setChecked(doctor.severalRHIF);

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
	ui.doctorList->clear();

	for (auto& doctor : doctors)
	{
        ui.doctorList->addItem(getDoctorName(doctor));
	}

	auto count = ui.doctorList->count();

	if (count) {
		ui.doctorList->setCurrentRow(count - 1);
		return;
	}

	//if list is empty
	ui.adminCheck->setHidden(true);
	ui.specialtyCombo->setHidden(true);
	ui.label_13->setHidden(true);
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
		c.unfavourable = ui.unfavCheck->isChecked();
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
	doctor.severalRHIF = ui.severalRHIFcheck->isChecked();

	return doctor;
}


void SettingsDialog::replaceCurrentItem(const PracticeDoctor& item)
{
    ui.doctorList->currentItem()->setText(getDoctorName(item));
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
