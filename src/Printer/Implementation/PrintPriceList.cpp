#include "PrintPrv.h"
#include "View/TableModels/ProcedureTemplateModel.h"

bool Print::priceList(ProcedureTemplateModel& m)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_prices.lrxml");

    auto& practice = User::practice();
    auto doctor = User::doctor();

    report.dataManager()->setReportVariable("practice", practice.name.c_str());
    report.dataManager()->setReportVariable("doctorName", doctor.getFullName(true).c_str());
    report.dataManager()->setReportVariable("date", QDateTime::currentDateTime().toString("dd.MM.yyyy г."));
    report.dataManager()->setReportVariable("bulstat", practice.bulstat.c_str());
    report.dataManager()->setReportVariable("city", practice.practice_address.getAddressWithStreet(" ").c_str());
    report.dataManager()->addModel("procedures", &m, false);

    QApplication::restoreOverrideCursor();

    report.printReport();
}
