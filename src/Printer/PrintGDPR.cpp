#include "PrintPrv.h"


void Print::gdpr(const Patient& patient, const std::string& pdfFilename)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_gdpr.lrxml");

    report.dataManager()->setReportVariable("practice_name", QString::fromStdString(practice.name));
    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address.getString()));
    report.dataManager()->setReportVariable("declarator", patient.sex ? "Долуподписаната" : "Долуподписаният");
    report.dataManager()->setReportVariable("type", patient.type == 1 ? "ЕГН" : "ЛНЧ");
    report.dataManager()->setReportVariable("name", QString::fromStdString(patient.fullName()));
    report.dataManager()->setReportVariable("id", QString::fromStdString(patient.id));
    report.dataManager()->setReportVariable("address", QString::fromStdString(patient.getFullAddress()));
    report.dataManager()->setReportVariable("acquainted", patient.sex ? "Запозната" : "Запознат");
    report.dataManager()->setReportVariable("date", Date::currentDate().toBgStandard(true).c_str());

    QApplication::restoreOverrideCursor();

    printLogic(report, pdfFilename);
}
