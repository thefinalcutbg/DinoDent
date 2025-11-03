#include "PrintPrv.h"

#include "View/TableModels/ProcedureTableModel.h"

bool Print::ambList(
    const AmbList& amb,
    const Patient& patient,
    bool printNhif,
    const std::vector<Procedure>& selectedProcedures,
    bool printReferrals,
    const std::string& pdfFilename
)
{

    //used as coordinates for the x-es in the checkboxes
    constexpr QChar tempSymbol{ 0x25EF };

   
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    report.loadFromFile(
        printNhif ?
        ":/reports/report_ambnhif.lrxml"
        :
        ":/reports/report_amb.lrxml"
    
    );

    auto& practice = User::practice();
    auto& doctor = User::doctor();


    if (patient.type < 5 && patient.type > 0)
    {
        report.dataManager()->setReportVariable("type" + QString::number(patient.type), "x");
    }

    if (
        practice.isUnfavourable() &&
        amb.isNhifSheet() &&
        amb.nhifData.isUnfavourable
        )
    {
        report.dataManager()->setReportVariable("unfav", "x");
    }

    PrintPrv::fillCommonData(report, patient, doctor, practice);

    report.dataManager()->setReportVariable("ambNum", QString::fromStdString(amb.nrn));

    report.dataManager()->setReportVariable("allergies", patient.getAllergiesStr().c_str());

    //getting other med stats:
    report.dataManager()->setReportVariable("currentDiseases",
        MedicalStatuses::toString(patient.medStats.condition).c_str()
    );

    report.dataManager()->setReportVariable("pastDiseases",
        MedicalStatuses::toString(patient.medStats.history).c_str()
    );


    std::array<bool, 32> temp;

    //assigning teeth status
    for (int i = 0; i < 32; i++)
    {
        QString tempVar = "temp" + QString::number(i);
        QString statusVar = "s" + QString::number(i);

        temp[i] = amb.teeth[i][Dental::Temporary];
        report.dataManager()->setReportVariable(tempVar, temp[i] ? QString{ tempSymbol } : QString{});

        QString status = QString::fromStdString(amb.teeth[i].getPrintStatus());

        report.dataManager()->setReportVariable(statusVar, status);

    }

    //assigning supernumeral status
    for (int i = 0; i < 4; i++)
    {
        SupernumeralPrint dsn(static_cast<Dental::Quadrant>(i), amb.teeth);

        if (!dsn.isValid()) continue;

        auto idx = QString::number(i);

        report.dataManager()->setReportVariable("dsnPos" + idx, dsn.position);
        report.dataManager()->setReportVariable("dsnS" + idx, dsn.printStatus.c_str());
    }

    //this is where NHIF and NON-NHIF sheets diverge

    if (!printNhif) {

        ProcedureTableModel pModel;

        pModel.setProcedures(selectedProcedures, true);

        report.dataManager()->addModel("procedures", &pModel, false);

        QApplication::restoreOverrideCursor();

        return PrintPrv::printLogic(report, pdfFilename);
    }

    //procedures
    for (size_t i = 0; i < 6 && i < selectedProcedures.size(); i++)
    {
        auto& p = selectedProcedures[i];

        QString idx = QString::number(i);

        QString nhifCode = p.financingSource == FinancingSource::NHIF ?
            QString::number(p.code.nhifCode()) : "";

        report.dataManager()->setReportVariable("pDate" + idx, p.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("pDiag" + idx, p.diagnosis.getDiagnosisText().c_str());
        report.dataManager()->setReportVariable("pTooth" + idx, p.getToothIndex().getNhifNumenclature().c_str());
        report.dataManager()->setReportVariable("pName" + idx, p.code.name().c_str());
        report.dataManager()->setReportVariable("pNhif" + idx, nhifCode);
        report.dataManager()->setReportVariable("pKsmp" + idx, p.code.ACHICode().c_str());

        if (p.code.type() == ProcedureType::Anesthesia)
        {
            report.dataManager()->setReportVariable("pMin" + idx, std::get<AnesthesiaMinutes>(p.param).minutes);
        }
    }

    //dental technician code

    if (amb.procedures.hasDentureManifactureProcedure() &&
        User::hasNhifContract()
        )
    {
        report.dataManager()->setReportVariable("iamn", User::practice().nhif_contract->dentalTechnicianCode.c_str());
    }

    //referrals

    if (printReferrals) {
        PrintPrv::fillOutReferrals(report, amb);
    }


    //setting denture declaration

    auto dentureDate = amb.procedures.nhifDentureDate();

    std::string dentureStr = "<b>Декларирам, че</b> ми е поставена ";

    dentureStr += amb.procedures.nhifDentureStr();

    dentureStr += " цяла плакова зъбна протеза на дата ";

    dentureStr += dentureDate.size() ? dentureDate : "....................... (подчертава се вярното)";

    report.dataManager()->setReportVariable("denture", dentureStr.c_str());

    QApplication::restoreOverrideCursor();

    return PrintPrv::printLogic(report, pdfFilename);
}

void Print::ambList()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_ambnhif.lrxml");

    report.dataManager()->setReportVariable("RZICode", QString::fromStdString(practice.rziCode));
    report.dataManager()->setReportVariable("specialty", doctor.specialtyAsInt());
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(doctor.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(doctor.getFullName(true)));

    report.dataManager()->setReportVariable("refType", QString{ "" });

    report.setShowProgressDialog(true);

    std::string dentureStr =
        "<b>Декларирам, че</b> ми е поставена горна и/или долна ";
    "цяла плакова зъбна протеза на дата ....................... (подчертава се вярното)";

    report.dataManager()->setReportVariable("denture", dentureStr.c_str());

    QApplication::restoreOverrideCursor();
    
    report.printReport();

}
