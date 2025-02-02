#include "PrintPrv.h"

void Print::printDentureDeclaration(const Patient& patient, DeclaratorType type, const AmbList* amblist, const std::string& pdfFilename)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    switch (type)
    {
    case DeclaratorType::Insured:
        report.loadFromFile(":/reports/report_dentureInsured.lrxml");
        break;
    case DeclaratorType::Custody:
        report.loadFromFile(":/reports/report_dentureCustody.lrxml");
        break;
    case DeclaratorType::Empty:
        report.loadFromFile(":/reports/report_dentureCustody.lrxml");
        break;
    }

    if (type != DeclaratorType::Empty) {
        fillCommonData(report, patient, User::doctor(), User::practice());
    }
    report.dataManager()->setReportVariable("denture", "горна и/или долна");
    report.dataManager()->setReportVariable("includeManifacture", "включва/не включва");

    if (amblist) {
        bool includeManifacture = amblist->procedures.hasDentureManifactureProcedure();
        report.dataManager()->setReportVariable("date", amblist->procedures.nhifDentureDate().c_str());
        report.dataManager()->setReportVariable("denture", amblist->procedures.nhifDentureStr().c_str());
        report.dataManager()->setReportVariable(
            "includeManifacture",
            includeManifacture ? "включва" : "не включва"
        );

        report.dataManager()->setReportVariable("city", User::practice().practice_address.getString().c_str());
    }

    QApplication::restoreOverrideCursor();

    printLogic(report, pdfFilename);

}