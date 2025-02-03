#include "PrintPrv.h"

bool Print::printDentureDeclaration(const Patient* patient, const AmbList* amblist, const std::string& pdfFilename)
{
    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_denture.lrxml");

    //default if empty declaration
    report.dataManager()->setReportVariable("declaratorType", "здравноосигурено лице, родител, настойник, попечител");
    report.dataManager()->setReportVariable("denture", "горна и/или долна");
    report.dataManager()->setReportVariable("includeManifacture", "включва/не включва");
    report.dataManager()->setReportVariable("priceAdded", "е добавена/не е добавена");
    report.dataManager()->setReportVariable("date", "................");

    if (patient == nullptr) return report.printReport();

    int declaratorType = ModalDialogBuilder::openButtonDialog(
        {
            "Здравноосигурено лице",
            "Родител, настойник, попечител"
        },
        "Декларация за активна здравна книжка",
        "Изберете декларатор:"
    );

    if (declaratorType == -1) return false;

    Patient declarator = *patient;

    if (declaratorType > 0) {

        PatientDialogPresenter p("Въведете родител/настойник/попечител");
        auto declaratorResult = p.open();

        if (!declaratorResult) return false;

        declarator = *declaratorResult;
    }

    QApplication::setOverrideCursor(Qt::BusyCursor);
    

    report.dataManager()->setReportVariable("declaratorType",
        declaratorType == 0 ?
        "здравноосигурено лице"
        :
        "родител, настойник, попечител"
    );

    QString date;

    if (amblist) {
        date = amblist->procedures.nhifDentureDate().c_str();
        bool includeManifacture = amblist->procedures.hasDentureManifactureProcedure();
        report.dataManager()->setReportVariable("denture", amblist->procedures.nhifDentureStr().c_str());
        report.dataManager()->setReportVariable(
            "includeManifacture",
            includeManifacture ? "включва" : "не включва"
        );

        report.dataManager()->setReportVariable(
            "priceAdded",
            includeManifacture ? "е добавена" : "не е добавена"
        );
    }

    report.dataManager()->setReportVariable("declaratorName", declarator.fullName().c_str());
    report.dataManager()->setReportVariable("declaratorId", declarator.id.c_str());
    report.dataManager()->setReportVariable("declaratorPhone", declarator.phone.c_str());
    report.dataManager()->setReportVariable("declaratorAddress", declarator.getFullAddress().c_str());

    if (declaratorType == 1) {
        report.dataManager()->setReportVariable("patientName", patient->fullName().c_str());
        report.dataManager()->setReportVariable("patientId", patient->id.c_str());
        report.dataManager()->setReportVariable("patientPhone", patient->phone.c_str());
        report.dataManager()->setReportVariable("patientAddress", patient->getFullAddress().c_str());
    }

    report.dataManager()->setReportVariable("doctorName", User::doctor().getFullName(false).c_str());
    report.dataManager()->setReportVariable("doctorName" + QString::number(declaratorType), User::doctor().getFullName(false).c_str());
    report.dataManager()->setReportVariable("date", date);
    report.dataManager()->setReportVariable("date" + QString::number(declaratorType), date);
    report.dataManager()->setReportVariable("city", User::practice().practice_address.getString().c_str());


    QApplication::restoreOverrideCursor();

    return printLogic(report, pdfFilename);

}