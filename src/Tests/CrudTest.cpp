#include "CrudTest.h"
#include "Database/Database.h"
#include "Database/DbPatient.h"
#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Database/DbAmbList.h"
#include "Model/User.h"
#include <QFile>

std::string Test::crudTest()
{

	bool success = true;

	Db::setFilePath("test.db");

	success = Db::createIfNotExist();

	if (!success) { return "Error creating database"; }

	//DbDoctor

	success = DbDoctor::insertDoctor(
		Doctor{
			.rowID{0},
			.LPK{"22"},
			.fname{"test"},
			.mname{"test"},
			.lname{"test"},
			.specialty{ NhifSpecialty::None },
			.pass{"test"},
			.phone{"test"},
			.severalRHIF{ false },
			.hisSpecialty{2081}
		}
	);

	if (!success) { return "Error inserting doctor"; };

	auto doctor = DbDoctor::getDoctor("22");

	if (!doctor.has_value()) { return "Error getting doctor"; }

	doctor->fname = "test1";

	success = DbDoctor::updateDoctor(*doctor, doctor->LPK);

	if (!success) return "Error updating doctor";

	doctor = DbDoctor::getDoctor("22");

	if (doctor->fname != "test1") return "Error updating doctor1";

	//DbPractice
	success = DbPractice::insertPractice(
		Practice{

				.rowID = 0,
				.rziCode = "21",
				.name = "test",
				.bulstat = "test",
				.firm_address = "test",
				.practice_address = 68134,
				.vat = "",
				.pass = "test",
				.legal_entity = 0,
				.selfInsuredId = "",
				.bank = "a",
				.iban = "b",
				.bic = "c",
				.settings{},
				.nhif_contract{}

		}
	);

	if (!success) { return "Error inserting practice"; };

	auto practice = DbPractice::getPractice("21");

	if (practice.rziCode.empty()) return "Error getting practice";

	practice.name = "test1";

	success = DbPractice::updatePractice(practice, "21");

	if (!success) return "Error updating practice";

	//DbPractice_doctor

	success = DbPractice::setDoctorsPracticeList(
		{
			PracticeDoctor{
				.lpk = "22",
				.name = "",
				.admin = true,
				.specialty = NhifSpecialty::General
			}
		}, "21"
	);

	if (!success) { return "Error setting practice_doctor list"; }

	auto [admin, specialty] = DbDoctor::getAdminAndSpecialty(doctor->LPK, practice.rziCode);

	if (!specialty) return "Error getting doctor specialty";
	if (!admin) return "Error getting doctor admin status";

	doctor->specialty = static_cast<NhifSpecialty>(specialty);

	User::setCurrentPractice(practice);
	User::setCurrentDoctor(*doctor);

	//DbPatient

	success = DbPatient::insert(
		Patient{
			.id = "0",
			.FirstName = "test",
			.MiddleName = "test",
			.LastName = "test"
		}
	);

	if (!success) return "Error inserting patient";

	auto patient = DbPatient::get("0", Patient::Type::EGN);

	if (!patient.rowid) return "Error getting patient";

	patient.FirstName = "test1";

	success = DbPatient::update(patient);

	if (!success)  return "Error updating patient";

	patient = DbPatient::get(patient.rowid);

	if (!patient.rowid) return "Error updating patient";

	if (patient.FirstName != "test1") return "Error updating patient";

	patient.medStats.condition.push_back("test");

	DbPatient::updateMedStatus(patient.rowid, patient.medStats);

	patient.medStats = DbPatient::getMedicalStatuses(patient.rowid);

	if (patient.medStats.condition.empty() || patient.medStats.condition[0] != "test") return "Medical statuses error";

	//DbAmbList

	AmbList list;
	list.date = Time::currentTime().to8601(Date::currentDate());
	list.patient_rowid = 1;
	list.LPK = "22";
	list.nrn = "test";
	list.number = 1;

	list.procedures.addProcedure(
		Procedure{
			.financingSource = FinancingSource::NHIF
		}
	);

	list.referrals.push_back(Referral(ReferralType::Form3));

	list.medical_notices.push_back(MedicalNotice());

	if (!DbAmbList::insert(list, 1)) return "Error inserting amb sheet";

	list = DbAmbList::getNewAmbSheet(1);

	if (list.nrn != "test") return "Error retrieveing new amb sheet";

	list.date = Time::currentTime().to8601(Date(1, 1, 2023));

	if (list.procedures.empty()) return "Error retrieveing procedures";
	if (list.referrals.empty()) return "Error retrieving referrals";
	if (list.medical_notices.empty()) return "Error retrieving medical notices";

	DbAmbList::update(list);

	list = DbAmbList::getNewAmbSheet(1);

	if (list.nrn.size()) return "Error getting new amb sheet or error with ambsheet update";

	if (DbAmbList::getMonthlyNhifSheets(1, 2023).empty()) {
		return "Error getting monthly sheets with procedures and referrals";
	}

	list = DbAmbList::getListData(1);

	list.procedures[0].financingSource = FinancingSource::Patient;

	DbAmbList::update(list);

	if (DbAmbList::getMonthlyNhifSheets(1, 2023).empty()) {
		return "Error getting monthly sheets with referrals";
	}

	list = DbAmbList::getListData(1);

	list.referrals = {};

	list.procedures[0].financingSource = FinancingSource::NHIF;

	DbAmbList::update(list);

	if (DbAmbList::getMonthlyNhifSheets(1, 2023).empty()) {
		return "Error getting monthly sheets with nhif referrals";
	}

	list.procedures[0].financingSource = FinancingSource::Patient;

	DbAmbList::update(list);

	if (DbAmbList::getMonthlyNhifSheets(1, 2023).size()) {
		return "Error getting monthly sheets that shouldn't be there";
	}

    if (DbAmbList::getNewNumber(Date(2, 1, 23)) != 1) {
		return "Error getting new ambsheet number";
	}

    QFile f("test.db");
    f.remove();

	return "CRUD test successful";

}
