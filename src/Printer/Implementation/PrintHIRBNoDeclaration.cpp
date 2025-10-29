#include "PrintPrv.h"

bool Print::printHirbNoDeclaration(const Patient* patient, const std::string& pdfFilename)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    
    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_hirbno.lrxml");

    //empty declaration
    if (patient == nullptr) {

        report.dataManager()->setReportVariable("declaratorType",
            "в качеството на: здравноосигурено лице; "
            "родител на малолетно/непълнолетно лице; настойник, попечител"
            "<br>(подчертава се съответното качество)"
        );

        return PrintPrv::printLogic(report, "");
    }

    //chose declarator type:

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
        auto declaratorResult = p.getPatient();

        if (!declaratorResult) return false;

        declarator = *declaratorResult;
    }
        
    report.dataManager()->setReportVariable("declaratorType",
        declaratorType == 0 ?
            "<b>в качеството на здравноосигурено лице</b>"
            :
            "<b>в качеството на родител на малолетно/непълнолетно лице, настойник, попечител</b>"
    );


    report.dataManager()->setReportVariable("declaratorName", declarator.fullName().c_str());
    report.dataManager()->setReportVariable("declaratorId", declarator.id.c_str());
    report.dataManager()->setReportVariable("declaratorName", declarator.fullName().c_str());
    report.dataManager()->setReportVariable("hirbNo" + QString::number(declaratorType), patient->HIRBNo.c_str());

    if (declaratorType == 1) {
        report.dataManager()->setReportVariable("patientName", patient->fullName().c_str());
        report.dataManager()->setReportVariable("patientId", patient->id.c_str());
    }

    report.dataManager()->setReportVariable("date", Date::currentDate().toBgStandard().c_str());
    report.dataManager()->setReportVariable("city", User::practice().practice_address.getString().c_str());

    QApplication::restoreOverrideCursor();

    return PrintPrv::printLogic(report, pdfFilename);

}