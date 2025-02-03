#include "PrintPrv.h"


bool Print::gdpr(const Patient& patient, const std::string& pdfFilename)
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

    return printLogic(report, pdfFilename);
}

void Print::gdpr()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_gdpr.lrxml");

    QString dots = "........................................";

    report.dataManager()->setReportVariable("practice_name", QString::fromStdString(practice.name));
    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address.getString()));
    report.dataManager()->setReportVariable("declarator", "Долуподписаният/та");
    report.dataManager()->setReportVariable("type", "ЕГН/ЛНЧ:");
    report.dataManager()->setReportVariable("name", dots);
    report.dataManager()->setReportVariable("id", dots);
    report.dataManager()->setReportVariable("address", dots);
    report.dataManager()->setReportVariable("acquainted", "Запознат/та");
    report.dataManager()->setReportVariable("date", ".................");

    QApplication::restoreOverrideCursor();

    report.printReport();
}