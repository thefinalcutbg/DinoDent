#include "PrintPrv.h"

void Print::printHirbNoDeclaration(const Patient& patient, DeclaratorType type, const std::string& pdfFilename)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    switch (type)
    {
    case DeclaratorType::Insured:
        report.loadFromFile(":/reports/report_nhifInsured.lrxml");
        break;
    case DeclaratorType::Custody:
        report.loadFromFile(":/reports/report_nhifCustody.lrxml");
        break;
    case DeclaratorType::Empty:
        report.loadFromFile(":/reports/report_nhifCustody.lrxml");
        break;
    }

    if (type != DeclaratorType::Empty) {
        fillCommonData(report, patient, User::doctor(), User::practice());
    }

    QApplication::restoreOverrideCursor();

    printLogic(report, pdfFilename);

}