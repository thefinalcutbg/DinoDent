#include "SettingsMainPresenter.h"

#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Database/Database.h"
#include "Database/DbPatient.h"
#include "Database/DbAmbList.h"
#include "Database/DbUpdateStatus.h"

#include "Network/GetHSM.h" 
#include "Network/NetworkManager.h"
#include "GlobalSettings.h"

#include "Model/User.h"
#include "Model/PlainTable.h"
#include "Model/Patient.h"

#include "Presenter/DoctorDialogPresenter.h"

#include "View/Widgets/SettingsDialog.h"
#include "View/SubWidgets/ProcedureListView.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/PriceInputDialog.h"

#include "Printer/FilePaths.h"
#include "Printer/Print.h"

SettingsMainPresenter::SettingsMainPresenter() :
	m_doctorsList(DbPractice::getDoctors(User::practice().rziCode)),
	monthlySheets(User::practice().generateMonthlySheets())
{}

void SettingsMainPresenter::setView(SettingsDialog* view)
{
	this->view = view;

	auto practice = DbPractice::getPractice(User::practice().rziCode);
	auto doctor = DbDoctor::getDoctor(User::doctor().LPK).value(); //quite unsafe lol

	view->setSettings(practice.settings);

	view->setGlobalSettings(GlobalSettings::getSettings());
	view->setDoctor(doctor);
	view->setPractice(practice);
	view->setDoctorList(m_doctorsList);

	setUpdateLabels();
	
	view->setAdminPriv(User::isAdmin());

	view->getPriceListView()->setPresenter(&procedure_list);

    view->getPriceListView()->showPrices();
}

void SettingsMainPresenter::priceEditRequested(const std::string& code)
{
    if(!ProcedureCode(code).isValid()) return;

    auto price = procedure_list.getPriceRange(code);

    PriceInputDialog d(price);

    auto result = d.getResult();

    if(!result) return;

    procedure_list.setCodePrice(code, result.value());

    view->getPriceListView()->refresh();
}

void SettingsMainPresenter::okPressed()
{
	if (applyChanges())
	{
		view->close();
	}
}

void SettingsMainPresenter::setUpdateLabels() {

	view->setUpdateDate(
		DynamicNum::Medication,
		DbUpdateStatus::lastUpdated(DynamicNum::Medication)
	);
}

void SettingsMainPresenter::updateMedications()
{
	med_update.setCallback
	(
		[&](bool success) { if (success) setUpdateLabels();}
	);

	med_update.update();
}

void SettingsMainPresenter::addDoctor()
{
	DoctorDialogPresenter p;

	auto doctor = p.open();

	if (!doctor.has_value()) return;

	for (auto& docPair : m_doctorsList)
	{
		if (docPair.lpk == doctor.value().LPK)
		{
			ModalDialogBuilder::showError("Този лекар вече е прибавен към практиката");
			return;
		}
	}

	m_doctorsList.emplace_back(PracticeDoctor{ doctor.value().LPK, doctor.value().getFullName(), false, NhifSpecialty::None });

	if (m_doctorsList.size() == 1) {
		m_doctorsList[0].admin = true;
	}

	view->setDoctorList(m_doctorsList);
}

void SettingsMainPresenter::removeDoctor()
{
	if (m_currentIndex < 0) return;

	if (User::isCurrentUser(m_doctorsList[m_currentIndex].lpk))
	{
		ModalDialogBuilder::showError("Не можете да изтриете профила от който сте влезли в момента");
		return;
	}

	m_doctorsList.erase(m_doctorsList.begin() + m_currentIndex);
	view->setDoctorList(m_doctorsList);
}

void SettingsMainPresenter::practiceDoctorChanged(int specialtyIdx, bool admin)
{
	m_doctorsList[m_currentIndex].admin = admin;
	m_doctorsList[m_currentIndex].specialty = static_cast<NhifSpecialty>(specialtyIdx);
	view->replaceCurrentItem(m_doctorsList[m_currentIndex]);
}

void SettingsMainPresenter::doctorIndexChanged(int index)
{
	m_currentIndex = index;

	if (m_currentIndex < 0) return;

	view->setDoctorProperties(m_doctorsList[index].admin, m_doctorsList[index].specialty);
}

bool SettingsMainPresenter::applyChanges()
{

	auto practice = view->getPractice();

	//rzi primary key db check
	if (practice.rziCode != User::practice().rziCode &&  //if the practice rzi has been changed
		DbPractice::practiceExists(practice.rziCode) //but the new rzi already exists in the db
		)
	{
		view->focusTab(SettingsDialog::Tab::Practice);
		ModalDialogBuilder::showError("Практика с такъв РЗИ номер вече съществува");
		return false;
	}

	auto doctor = view->getDoctor();

	//lpk primary key db check
	if (doctor.LPK != User::doctor().LPK &&
		DbDoctor::suchDoctorExists(doctor.LPK)
		)
	{
		view->focusTab(SettingsDialog::Tab::Doctor);
		ModalDialogBuilder::showError(
			"Доктор с такъв УИН вече съществува. За да го добавите, "
			"използвайте \"Добави лекар\" от раздел \"Практика\". "
			"След това излезте от текущия профил и влезте с УИН-а на другия лекар."
		);
		return false;

	}

	//lame, the state of the practice has to be kept in the presenter
	if (m_doctorsList.size() &&
		m_doctorsList[0].specialty == NhifSpecialty::None &&
		practice.nhif_contract
		)
	{

		bool atLeastOneNhifDoctor{ false };

		for (auto& d : m_doctorsList) {

			if (d.specialty != NhifSpecialty::None) {
				atLeastOneNhifDoctor = true;
				break;
			}
		}

		if (atLeastOneNhifDoctor == false) {

			bool answer = ModalDialogBuilder::askDialog(
				"Практиката има договор с НЗОК, но нито един лекар в нея\n"
				"няма код специалност по НЗОК. Да продължа ли въпреки това?");
			if (!answer) {
				view->focusTab(SettingsDialog::Tab::Practice);
				return false;
			}

		}

	}

	bool hasAdmin = false;

	for (auto& doctor : m_doctorsList) {
		if (doctor.admin) {
			hasAdmin = true;
			break;
		}
	}

	if (!hasAdmin) {
		view->focusTab(SettingsDialog::Tab::Practice);
		ModalDialogBuilder::showError("Практиката трябва да има поне един администратор");
		return false;
	}

	practice.settings = view->getSettings();

	DbPractice::updatePractice(practice, User::practice().rziCode);
	DbPractice::setDoctorsPracticeList(m_doctorsList, practice.rziCode);
	DbPractice::updatePracticeSettings(practice.settings, practice.rziCode);
	DbDoctor::updateDoctor(doctor, User::doctor().LPK);

	User::setCurrentPractice(DbPractice::getPractice(practice.rziCode));
	User::setCurrentDoctor(DbDoctor::getDoctor(doctor.LPK).value());
	User::refereshPracticeDoctor();
	NetworkManager::setTimeout(practice.settings.timeout);

	auto globalData = view->getGlobalSettings();

	PKCS11::setDriverPaths(globalData.pkcs11_list);
	User::signatureTablet() = SignatureTablet(globalData.signer_model, globalData.signer_filepath);
	FilePaths::setSettings(globalData.pdfDir, globalData.subdirStructure);

	GlobalSettings::setSettings(globalData);

	//if nhif contract has just been changed show monthly/dayly amb sheeet message
	if (monthlySheets != practice.generateMonthlySheets()) {

		auto message = practice.generateMonthlySheets() ? 
			"Амбулаторните листове ще бъдат генерирани на месечна основа."
			:
			"Амбулаторните листове ще бъдат генерирани на дневна основа.";
		
		ModalDialogBuilder::showMessage(message);
	}

	return true;
}

void SettingsMainPresenter::sqlCommandExec(const std::string& sql)
{

	Db db(sql);

	int columnCount = db.columnCount();

	if (!db.columnCount()) {

		if (!db.execute()) return;

		ModalDialogBuilder::showMessage(
			"Заявката е изпълнена успешно!\n"
			"Брой засегнати редове: " +
			std::to_string(db.rowsAffected())
		);

		return;
	}

	PlainTable table;

	for (int i = 0; i < columnCount; i++) {
		table.addColumn(PlainColumn(db.columnName(i)));
	}

	while (db.hasRows()) {
		for (int i = 0; i < columnCount; i++) {
            table.addCell(i, PlainCell{db.asString(i)});
		}
	}
	
	view->setSqlTable(table);
}

void SettingsMainPresenter::printEmptyDocs()
{
	static std::vector<std::string> printOptions{
			"Амбулаторен лист",
			"Декларация за тотални протези",
			"Декларация за валидна здравна книжка",
			"Информирано съгласие",
			"Декларация GDPR"
	};

	int result = ModalDialogBuilder::openButtonDialog(printOptions, "Печат на празни бланки");

	switch (result)
	{
		case 0: Print::ambList(); break;
		case 1: Print::printDentureDeclaration(nullptr); break;
		case 2: Print::printHirbNoDeclaration(nullptr); break;
		case 3: Print::consent(); break;
		case 4: Print::gdpr(); break;
	}

}

void SettingsMainPresenter::practiceTabFocused()
{
    view->setDoctorList(m_doctorsList);
}

void SettingsMainPresenter::hisImport()
{

	auto nrn = ModalDialogBuilder::inputDialog(
		"Въведете НРН:",
		"Зареждане на амбулаторен лист от НЗИС"
	);

	if (!nrn.size()) return;

	his_fetch_service.sendRequest(nrn, importToDb);
}

void SettingsMainPresenter::balanceRequested(const std::string& usr, const std::string& pass)
{
	sms_balance_service.checkBalance(usr, pass);
}

void SettingsMainPresenter::importToDb(const AmbList& amb, const Patient& p)
{
	if(amb.LPK != User::doctor().LPK) {
		ModalDialogBuilder::showError("Амбулаторният лист не е издаден от текущия лекар.");
		return;
	}

	Patient patient = p;

	patient.rowid = DbPatient::getPatientRowid(p.id, p.type);

	if (patient.rowid) {

		if (ModalDialogBuilder::askDialog(
			"Този пациент вече съществува в локалната база данни. Желаете ли презапишете данните му?"
		)) {
			DbPatient::update(patient);
		}
	}
	else {
		patient.rowid = DbPatient::insert(patient);
	}

	AmbList ambSheet = amb;

	ambSheet.rowid = DbAmbList::getRowidByNRN(ambSheet.nrn);
	ambSheet.patient_rowid = patient.rowid;

	bool success = false;

	if (ambSheet.rowid &&
		ModalDialogBuilder::askDialog(
			"Този амбулаторен лист вече съществува в базата данни. Желаете ли да го презапишете?"
		)
	){
		DbAmbList::update(ambSheet);
		success = true;
	}
	else {
		success = DbAmbList::insert(ambSheet, ambSheet.patient_rowid);
	}

	if (!success) return;

	ModalDialogBuilder::showMessage("Импортирането на амбулаторен лист на " + patient.firstLastName() + " приключи успешно.");
	
}
