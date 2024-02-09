#include "Printer.h"

#include <LimeReport/include/lrreportengine.h>
#include <QApplication>
#include <QString>

#include "View/Widgets/ProcedurePrintSelectDialog.h"
#include "View/TableModels/BusinessOperationModel.h"
#include "View/GlobalFunctions.h"
#include "View/Widgets/InvoicePrintDialog.h"
#include "View/TableModels/FiscalReceiptTableModel.h"
#include "View/TableModels/PlainTableModel.h"

#include "Model/User.h"
#include "Model/FreeFunctions.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/Financial/Invoice.h"
#include "Model/Referrals/Referral.h"
#include "Model/Dental/SupernumeralPrint.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/NhifSpecReport.h"

void fillCommonData(LimeReport::ReportEngine& report, const Patient& patient, const Doctor& doctor, const Practice& practice)
{
    report.dataManager()->setReportVariable(patient.type > Patient::LNCH ? "ssn" : "id", QString::fromStdString(patient.id));
    
    if (patient.foreigner)
    {
        report.dataManager()->setReportVariable("country", patient.foreigner->country.getCode().c_str());
    }

    report.dataManager()->setReportVariable("city", QString::fromStdString(patient.city.getString()));
    report.dataManager()->setReportVariable("address", QString::fromStdString(patient.address));
    report.dataManager()->setReportVariable("patientName", QString::fromStdString(patient.fullName()));
    report.dataManager()->setReportVariable("patientPhone", QString::fromStdString(patient.phone));
    report.dataManager()->setReportVariable("RHIFCode", QString::fromStdString(patient.city.getRhif()));
    report.dataManager()->setReportVariable("healthRegion", QString::fromStdString(patient.city.getHealthRegion()));
    report.dataManager()->setReportVariable("birth", QString::fromStdString(patient.birth.toBgStandard()));
    report.dataManager()->setReportVariable("RZICode", QString::fromStdString(practice.rziCode));
    report.dataManager()->setReportVariable("specialty", doctor.specialtyAsInt());
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(doctor.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(doctor.getFullName(true)));
    report.dataManager()->setReportVariable("hirbNo", QString::fromStdString(patient.HIRBNo));
}

void Print::ambList(const AmbList& amb, const Patient& patient)
{

    //used as coordinates for the x-es in the checkboxes
    struct coords { int x{ 0 }; int y{ 0 }; };
    constexpr coords typeCoords[5]{ {}, { 50, 213 }, { 225, 213 }, {50, 255}, {225, 255} };
    constexpr QChar tempSymbol{ 0x25EF };

    std::vector<Procedure> selectedProcedures;

    if (amb.procedures.size() || amb.referrals.size())
    {
        ProcedurePrintSelectDialog dialog(amb.procedures.list(), amb.referrals);

        for (auto& p : amb.procedures) {
            if (p.isNhif()) {
                dialog.selectNhifOnly(true);
                break;
            }
        }

        if (dialog.exec() == QDialog::Rejected) {
            return;
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
	

    if (patient.type < 5)
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

        report.dataManager()->setReportVariable("pDate" + idx , p.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("pDiag" + idx, p.diagnosis.getFullDiagnosis().c_str());
        report.dataManager()->setReportVariable("pTooth" + idx, p.tooth_idx.getNhifNumenclature().c_str());
        report.dataManager()->setReportVariable("pName" + idx, p.code.name().c_str());
        report.dataManager()->setReportVariable("pNhif" + idx, QString::number(p.code.oldCode()));
        report.dataManager()->setReportVariable("pKsmp" + idx, p.financingSource == FinancingSource::NHIF ? p.code.ksmp().c_str() : "");

        if(p.code.type() == ProcedureType::anesthesia)
        {
            report.dataManager()->setReportVariable("pMin" + idx, std::get<AnesthesiaMinutes>(p.result).minutes);
        }

    }

    const Referral *form3{ nullptr },
                   *form3a{ nullptr },
                   *mdd4_1{ nullptr },
                   *mdd4_2{ nullptr },
                   *mh119{ nullptr };

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


    QApplication::restoreOverrideCursor();

   // report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
   // report.setPreviewPageBackgroundColor(QColor(Qt::white));
   // report.previewReport(LimeReport::PreviewHint::HidePreviewStatusBar);
    report.printReport();
}

void Print::invoice(const Invoice& inv)
{

    InvoicePrintDialog d;
    d.exec();

    auto result = d.result();

    if (result == InvoicePrintDialog::Canceled) return;

    bool original = result == InvoicePrintDialog::Original ? true : false;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    auto report = LimeReport::ReportEngine();
    report.loadFromFile(":/reports/report_invoice.lrxml");

    report.dataManager()->setReportVariable("title", QString::fromStdString(inv.name));
    report.dataManager()->setReportVariable("number_date", QString::fromStdString(" № " + inv.getInvoiceNumber() + " от дата " + inv.date.toBgStandard(true)));
    report.dataManager()->setReportVariable("invoice_type", original ? "ОРИГИНАЛ" : "КОПИЕ");

    auto mainDoc = inv.mainDocument();

    if (mainDoc) {
        report.dataManager()->setReportVariable(
            "main_document",
            QString::fromStdString(
                "към фактура № "
                + FreeFn::leadZeroes(mainDoc->number, 10)
                + " от " + mainDoc->date.toBgStandard()
                + "г."));
    }

    auto issuer = inv.issuer();

    report.dataManager()->setReportVariable("recipient_name", QString::fromStdString(inv.recipient.name));
    report.dataManager()->setReportVariable("recipient_address", QString::fromStdString(inv.recipient.address));
    report.dataManager()->setReportVariable("recipient_bulstat", QString::fromStdString(inv.recipient.bulstat));
    report.dataManager()->setReportVariable("recipient_phone", QString::fromStdString(inv.recipient.phone));

    report.dataManager()->setReportVariable("issuer_name", QString::fromStdString(issuer.company_name));
    report.dataManager()->setReportVariable("issuer_address", QString::fromStdString(issuer.address_by_contract));
    report.dataManager()->setReportVariable("issuer_bulstat", QString::fromStdString(issuer.bulstat));

    report.dataManager()->setReportVariable("total", formatDoubleWithDecimal(inv.aggragated_amounts.total_amount));

    if (inv.nhifData.has_value())
    {
        report.dataManager()->setReportVariable("practice_rzi", QString::fromStdString(inv.nhifData->rhi_nhif_no));
        report.dataManager()->setReportVariable("contract", QString::fromStdString(inv.nhifData->contract_no + " / " + inv.nhifData->contract_date.toBgStandard()) + " г.");
        report.dataManager()->setReportVariable("mon_notif_number", QString::number(inv.nhifData->fin_document_month_no));
        report.dataManager()->setReportVariable("period", QString::fromStdString("от " + inv.nhifData->date_from.toBgStandard() + " до " + inv.nhifData->date_to.toBgStandard()));

    }

    BusinessOperationModel model{ inv.businessOperations };
    report.dataManager()->addModel("operations", &model, false);

    report.dataManager()->setReportVariable("taxEventDate", QString::fromStdString(inv.aggragated_amounts.taxEventDate.toBgStandard()));
    report.dataManager()->setReportVariable("madeBy", QString::fromStdString(User::doctor().getFullName(false)));
    report.dataManager()->setReportVariable("groundsNoVAT", QString::fromStdString(issuer.grounds_for_not_charging_VAT));
    report.dataManager()->setReportVariable("paymentType", QString::fromStdString(
        inv.aggragated_amounts.paymentType == PaymentType::Bank ?
        "Банков път"
        :
        "В брой"
    ));
    
    QApplication::restoreOverrideCursor();

   // report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
   // report.setPreviewPageBackgroundColor(QColor(Qt::white));
   // report.previewReport(LimeReport::PreviewHint::HidePreviewStatusBar);

    report.printReport();
}

void Print::consent(const Patient& patient)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = User::doctor();
    auto& practice = User::practice();

    report.loadFromFile(":/reports/report_consent.lrxml");

    const char* idType[5] = { "","ЕГН","ЛНЧ","ССН","" };

    report.dataManager()->setReportVariable("practice_name", QString::fromStdString(practice.name));
    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address.getString()));
    report.dataManager()->setReportVariable("doctor", QString::fromStdString(doctor.getFullName()));
    report.dataManager()->setReportVariable("declarator", patient.sex ? "Долуподписаната" : "Долуподписаният");
    report.dataManager()->setReportVariable("type", idType[patient.type]);
    report.dataManager()->setReportVariable("name", QString::fromStdString(patient.fullName()));
    report.dataManager()->setReportVariable("id", QString::fromStdString(patient.id));

    report.dataManager()->setReportVariable("phone", QString::fromStdString(patient.phone));


    QApplication::restoreOverrideCursor();

    report.printReport();
}

void Print::gdpr(const Patient& patient)
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

    report.printReport();
}

void Print::ambList()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = User::doctor();
    auto& practice = User::practice();
    
    report.loadFromFile(":/reports/report_amb.lrxml");

    report.dataManager()->setReportVariable("RZICode", QString::fromStdString(practice.rziCode));
    report.dataManager()->setReportVariable("specialty", doctor.specialtyAsInt());
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(doctor.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(doctor.getFullName(true)));

    report.dataManager()->setReportVariable("refType", QString{ "" });

    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();

  //  report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
  //  report.setPreviewPageBackgroundColor(QColor(Qt::white));
  //  report.previewReport(LimeReport::PreviewHint::HidePreviewStatusBar);

    report.printReport();

}

void Print::printDentureDeclaration(const Patient& patient, DeclaratorType type)
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

    QApplication::restoreOverrideCursor();
    report.printReport();

}

void Print::printHirbNoDeclaration(const Patient& patient, DeclaratorType type)
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
    report.printReport();

}

void Print::referral(const Referral& ref, const Patient& patient, const std::string& ambSheetNumber)
{

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    
    auto refCommonFill = [&] {
        report.dataManager()->setReportVariable("ambNum", ambSheetNumber.c_str());
        report.dataManager()->setReportVariable("refNum", ref.getNumber().c_str());
        report.dataManager()->setReportVariable("diagnosis_main", ref.diagnosis.main.code().c_str());
        report.dataManager()->setReportVariable("diagnosis_additional", ref.diagnosis.additional.code().c_str());
        report.dataManager()->setReportVariable("reason", FreeFn::leadZeroes(ref.reason.getIndex() + 1, 2).c_str());
        report.dataManager()->setReportVariable("date", ref.date.toBgStandard().c_str());

    };

    auto mdd4fill = [&] {

        auto& doctor = User::doctor();
        auto& practice = User::practice();
        auto data = std::get<MDD4Data>(ref.data);

        report.loadFromFile(data.isOPG() ? ":/reports/report_mdd4.lrxml"
        : ":/reports/report_mdd4tooth.lrxml");

        fillCommonData(report, patient, doctor, practice);

        if (patient.type == Patient::LNCH) {
            report.dataManager()->setReportVariable("type", "X");
        }

        refCommonFill();
        /*
        report.dataManager()->setReportVariable("ambNum", QString::fromStdString(FreeFn::leadZeroes(ambSheetNumber, 12)));
        report.dataManager()->setReportVariable("refNum", QString::fromStdString(FreeFn::leadZeroes(ref.number, 12)));
        report.dataManager()->setReportVariable("diagnosis_main", ref.diagnosis.main.code().c_str());
        report.dataManager()->setReportVariable("diagnosis_additional", ref.diagnosis.additional.code().c_str());
        report.dataManager()->setReportVariable("reason", FreeFn::leadZeroes(ref.reason.getIndex() + 1, 2).c_str());
        */

        report.dataManager()->setReportVariable("nhifCode", data.getCode().c_str());
        report.dataManager()->setReportVariable("KSMP", data.getKSMP().c_str());

        if (!data.isOPG()) {
            report.dataManager()->setReportVariable("tooth", ToothUtils::getToothNumber(data.tooth_idx, false));
        }

    };

    auto mh119fill = [&] {

        
        report.loadFromFile(":/reports/report_mh119.lrxml");

        refCommonFill();

        fillCommonData(report, patient, User::doctor(), User::practice());

        auto data = std::get<MH119Data>(ref.data);

        report.dataManager()->setReportVariable("motives", data.description.c_str());
        report.dataManager()->setReportVariable("doctorPhone", User::doctor().phone.c_str());
        report.dataManager()->setReportVariable("diagnosis", ref.diagnosis.getText().c_str());
        report.dataManager()->setReportVariable("comorbidity", ref.comorbidity.getText().c_str());
        
        switch (data.specCode)
        {
            case MH119Data::Pediatric:
                report.dataManager()->setReportVariable("specCode", "61");
                break;
            case MH119Data::Surgery:
                report.dataManager()->setReportVariable("specCode", "62/68");
                break;
        }
        
        report.dataManager()->setReportVariable("practiceName", User::practice().name.c_str());
        
        int typeYpos[4] = { 440, 480, 525, 570 };

        report.dataManager()->setReportVariable("typeY", typeYpos[data.reason.getIndex()]);

    };

    switch (ref.type)
    {
        case ReferralType::MDD4:
            mdd4fill();
            break;
        case ReferralType::MH119:
            mh119fill();
            break;
        default:
            QApplication::restoreOverrideCursor();
            return;
    }

    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();
    report.printReport();


}

void Print::saveFsicalReportToPDF(const FiscalReport& r)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_fiscalDescr.lrxml");

    FiscalReceiptTableModel model;
    model.setRows(r.receipts);

    report.dataManager()->setReportVariable("dateFrom", r.dateFrom.c_str());
    report.dataManager()->setReportVariable("dateTo", r.dateTo.c_str());
    report.dataManager()->addModel("receipts", &model, false);

    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();
    report.printToPDF(r.filename.c_str());


}

void Print::saveNhifSpecReport(const NhifSpecReport& spec_report)
{
    PlainTableModel tableModel;
    tableModel.setTableData(spec_report.getSpecificationReport());
    
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_nhifSpec.lrxml");

    report.dataManager()->setReportVariable("doctorName", spec_report.doctor_name.c_str());
    report.dataManager()->setReportVariable("dateFrom", spec_report.dateFrom.toBgStandard().c_str());
    report.dataManager()->setReportVariable("dateTo", spec_report.dateTo.toBgStandard().c_str());
    report.dataManager()->setReportVariable("specType", spec_report.getSpecString().c_str());
    report.dataManager()->addModel("receipts", &tableModel, false);

    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();
    report.printReport();
}

// void Print::ambList(const AmbList &amb, const Patient &patient)
// {

// }

// void Print::invoice(const Invoice &inv)
// {

// }

// void Print::consent(const Patient &patient)
// {

// }

// void Print::gdpr(const Patient &patient)
// {

// }

// void Print::ambList()
// {

// }

// void Print::printDentureDeclaration(const Patient &patient, DeclaratorType type)
// {

// }

// void Print::printHirbNoDeclaration(const Patient &patient, DeclaratorType type)
// {

// }

// void Print::referral(const Referral &ref, const Patient &patient, const std::string &ambSheetNumber)
// {

// }

// void Print::saveFsicalReportToPDF(const FiscalReport &report)
// {

// }
