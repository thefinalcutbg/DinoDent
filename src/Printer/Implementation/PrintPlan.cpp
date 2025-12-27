#include "PrintPrv.h"

bool Print::treatmentPlan(const TreatmentPlan& plan, const Patient& patient, const std::string& pdfFilename)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_plan.lrxml");


    auto& practice = User::practice();
    auto& doctor = User::doctor();

    PrintPrv::fillCommonData(report, patient, doctor, practice);

    QApplication::restoreOverrideCursor();

    report.designReport();

    return PrintPrv::printLogic(report, pdfFilename);
}
