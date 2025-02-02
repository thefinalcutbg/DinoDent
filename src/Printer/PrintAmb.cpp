#include "PrintPrv.h"

bool Print::ambList(const AmbList& amb, const Patient& patient, const std::string& pdfFilename)
{

    //used as coordinates for the x-es in the checkboxes
    struct coords { int x{ 0 }; int y{ 0 }; };
    constexpr coords typeCoords[5]{ {0, 0}, { 50, 213 }, { 225, 213 }, {50, 255}, {225, 255} };
    constexpr QChar tempSymbol{ 0x25EF };

    std::vector<Procedure> selectedProcedures;

    if (amb.procedures.size() || amb.referrals.size())
    {
        ProcedurePrintSelectDialog dialog(amb.procedures.list(), amb.referrals);

        for (auto& p : amb.procedures) {
            if (p.isNhif()) {
                dialog.selectFinancingSource(FinancingSource::NHIF);
                break;
            }
        }

        if (dialog.exec() == QDialog::Rejected) {
            return false;
        }

        auto selectedIndexes = dialog.selectedProcedures();

        for (auto idx : selectedIndexes) {
            selectedProcedures.push_back(amb.procedures.at(idx));
        }

    }

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    report.loadFromFile(":/reports/report_amb.lrxml");

    auto& practice = User::practice();
    auto& doctor = User::doctor();


    if (patient.type < 5 && patient.type > 0)
    {
        report.dataManager()->setReportVariable("type", QString{ "x" });
        report.dataManager()->setReportVariable("typeX", typeCoords[patient.type].x);
        report.dataManager()->setReportVariable("typeY", typeCoords[patient.type].y);
    }

    if (
        practice.isUnfavourable() &&
        amb.isNhifSheet() &&
        amb.nhifData.isUnfavourable
        )
    {
        report.dataManager()->setReportVariable("unfav", "x");
    }

    fillCommonData(report, patient, doctor, practice);

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

    const Referral* form3{ nullptr },
        * form3a{ nullptr },
        * mdd4_1{ nullptr },
        * mdd4_2{ nullptr },
        * mh119{ nullptr };

    for (auto& ref : amb.referrals)
    {
        switch (ref.type)
        {
        case ReferralType::Form3: form3 = &ref; break;
        case ReferralType::Form3A: form3a = &ref; break;
        case ReferralType::MH119: mh119 = &ref; break;
        case ReferralType::MDD4: mdd4_1 ? mdd4_2 = &ref : mdd4_1 = &ref; break;
        }
    }

    if (mdd4_1)
    {
        auto& ref = *mdd4_1;
        auto& mddData = std::get<MDD4Data>(ref.data);

        report.dataManager()->setReportVariable("mdd4Num1", ref.nrn.c_str());
        report.dataManager()->setReportVariable("mdd4Date1", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("mdd4Ksmp1", mddData.getKSMP().c_str());
        report.dataManager()->setReportVariable("mdd4Nhif1", mddData.getCode().c_str());
    }

    if (mdd4_2)
    {
        auto& ref = *mdd4_2;
        auto& mddData = std::get<MDD4Data>(ref.data);

        report.dataManager()->setReportVariable("mdd4Num2", ref.nrn.c_str());
        report.dataManager()->setReportVariable("mdd4Date2", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("mdd4Ksmp2", mddData.getKSMP().c_str());
        report.dataManager()->setReportVariable("mdd4Nhif2", mddData.getCode().c_str());
    }


    if (mh119)
    {
        auto& ref = *mh119;

        switch (std::get<MH119Data>(ref.data).specCode)
        {
        case MH119Data::Pediatric:
            report.dataManager()->setReportVariable("mh119SpecCode1", 61);
            report.dataManager()->setReportVariable("mh119Date1", ref.date.toBgStandard().c_str());
            break;
        case MH119Data::Surgery:
            report.dataManager()->setReportVariable("mh119SpecCode2", 62);
            report.dataManager()->setReportVariable("mh119SpecCode3", 68);
            report.dataManager()->setReportVariable("mh119Date2", ref.date.toBgStandard().c_str());
            break;
        }
    }

    if (form3)
    {

        report.dataManager()->setReportVariable("ref3x", "X");

        auto& ref = *form3;

        auto& refData = std::get<R3Data>(ref.data);

        report.dataManager()->setReportVariable("refNum", ref.nrn.c_str());
        report.dataManager()->setReportVariable("refDate", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("refSpecCode", refData.specialty);
        report.dataManager()->setReportVariable("refMkb", ref.diagnosis.main.code().c_str());
    }

    if (form3a)
    {
        report.dataManager()->setReportVariable("ref3Ax", "X");

        auto& ref = *form3a;

        auto& refData = std::get<R3AData>(ref.data);

        report.dataManager()->setReportVariable("refNum", ref.nrn.c_str());
        report.dataManager()->setReportVariable("refDate", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("refSpecCode", refData.nhifSpecialty);
        report.dataManager()->setReportVariable("refMkb", ref.diagnosis.main.code().c_str());
        report.dataManager()->setReportVariable("refKSMP", refData.ksmp);
        report.dataManager()->setReportVariable("refHSA", refData.highlySpecializedActivity);
    }

    //setting denture declaration

    auto dentureDate = amb.procedures.nhifDentureDate();

    std::string dentureStr = "<b>Декларирам, че</b> ми е поставена ";

    dentureStr += amb.procedures.nhifDentureStr();

    dentureStr += " цяла плакова зъбна протеза на дата ";

    dentureStr += dentureDate.size() ? dentureDate : "....................... (подчертава се вярното)";

    report.dataManager()->setReportVariable("denture", dentureStr.c_str());

    QApplication::restoreOverrideCursor();

    return printLogic(report, pdfFilename);
}

void Print::ambList()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_amb.lrxml");

    report.dataManager()->setReportVariable("type", QString{ "" });
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
