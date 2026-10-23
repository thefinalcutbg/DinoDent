#include "PrintPrv.h"
#include "Database/DbPatient.h"
#include "Model/User.h"
#include "Model/SickLeave.h"

bool Print::sickLeave(const Patient& p, const SickLeave& sickLeave, const std::string& nrn, const std::string filename) {

	 auto report = LimeReport::ReportEngine();

	 auto recipient = sickLeave.caregiver_rowid ? DbPatient::get(sickLeave.caregiver_rowid) : p;

     report.loadFromFile(":/reports/report_sickLeave.lrxml");

	 PrintPrv::fillCommonData(report, recipient, User::doctor(), User::practice());

	 report.dataManager()->setReportVariable("sickNumber", "E" +  QString::number(sickLeave.number));
	 report.dataManager()->setReportVariable("nrn", nrn.c_str());
	 report.dataManager()->setReportVariable("address_by_activity", User::practice().practice_address.getString().c_str());
	 report.dataManager()->setReportVariable(recipient.type == Patient::EGN ? "egn" : "lnch", "X");
	 report.dataManager()->setReportVariable(recipient.type == Patient::Male ? "male" : "female", "X");
	 report.dataManager()->setReportVariable(sickLeave.isContinuation ? "continuation" : "primary", "X");
	 report.dataManager()->setReportVariable("age", recipient.getAge());
	 report.dataManager()->setReportVariable("employer", sickLeave.employerName.c_str());
	 report.dataManager()->setReportVariable("e_address", sickLeave.employerCity.getString().c_str());
	 report.dataManager()->setReportVariable("profession", sickLeave.profession.c_str());
	 report.dataManager()->setReportVariable("position", sickLeave.position.c_str());

	 report.dataManager()->setReportVariable("fromDate", sickLeave.startDate.toBgStandard().c_str());
	 report.dataManager()->setReportVariable("toDate", sickLeave.endDate.toBgStandard().c_str());
	 report.dataManager()->setReportVariable("checkupDate", sickLeave.checkup.toBgStandard().c_str());
	 report.dataManager()->setReportVariable("date", sickLeave.date.toBgStandard().c_str());

	 report.dataManager()->setReportVariable("diagnosis", sickLeave.diagnosis.name().c_str());
	 report.dataManager()->setReportVariable("icd", sickLeave.diagnosis.code().c_str());

	 report.dataManager()->setReportVariable("r" + QString::number(sickLeave.regimeType), "X");
	 report.dataManager()->setReportVariable("reasonCode", QString::number(sickLeave.reasonCode));
	 report.dataManager()->setReportVariable("reason", sickLeave.reasons.at(sickLeave.reasonCode).c_str());

	 report.dataManager()->setReportVariable("daysWords", sickLeave.dayCountWords().c_str());
	 report.dataManager()->setReportVariable("days", QString::number(sickLeave.daysCount()));

	 if (sickLeave.caregiver_rowid) {
		 report.dataManager()->setReportVariable("sickName", p.fullName().c_str());
		 report.dataManager()->setReportVariable("sickId", p.id.c_str());
		 report.dataManager()->setReportVariable(p.type == Patient::EGN ? "sickEgn" : "sickLnch", "X");
		 report.dataManager()->setReportVariable("relationship", sickLeave.relationships.at(sickLeave.relationship).c_str());
	 }

	 auto dayCount = sickLeave.daysCount();

	 QString variable;

	 switch (sickLeave.regimeType)
	 {
		 case 1: variable = "hospitalDays"; break;
		 case 2: case 3: case 4: case 5: variable = "homeDays"; break;
		 case 6: variable = "sanDays"; break;
		 default: variable = "freeDays"; break;
	 }

	 report.dataManager()->setReportVariable(variable, QString::number(sickLeave.daysCount()));

	 QString notes;
	 if (sickLeave.noteCode) {
		 notes += sickLeave.notes.at(sickLeave.noteCode).c_str();
	     notes += ". ";
	 }
	 
	 notes += sickLeave.additionalNote.c_str();

	 report.dataManager()->setReportVariable("notes", notes);

	 return PrintPrv::printLogic(report, filename);

}