#include "Printer.h"
#include <LimeReport/lrreportengine.h>
#include <QString>
#include <QApplication>
#include "View/Widgets/ProcedurePrintSelectDialog.h"
#include "View/Models/ProcedureTableModel.h"
#include "Model/FreeFunctions.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/Financial/Invoice.h"
#include "View/Models/BusinessOperationModel.h"
#include "View/GlobalFunctions.h"
#include "qitemselectionmodel.h"
#include "Model/User.h"
#include "View/Widgets/InvoicePrintDialog.h"
#include "Model/Referrals/Referral.h"

void fillCommonData(LimeReport::ReportEngine& report, const Patient& patient, const Doctor& doctor, const Practice& practice)
{

    report.dataManager()->setReportVariable("id", QString::fromStdString(patient.id));
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
    struct coords { int x; int y; };
    constexpr coords typeCoords[5]{ {0, 0}, { 50, 213 }, { 225, 213 }, {50, 255}, {225, 255} };
    constexpr QChar tempSymbol{ 0x25EF };

    std::vector<Procedure> selectedProcedures;


    ProcedurePrintSelectDialog dialog(amb.procedures.list(), amb.referrals);
        
    for (auto& p : amb.procedures) {
        if (p.nhif) {
            dialog.selectOnlyWhereNzokIs(true);
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



    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    report.loadFromFile(":/reports/report_amb.lrxml");
    
    auto& practice = User::practice();
    auto& doctor = User::doctor();
	

    if (patient.type < 5);
    {
        report.dataManager()->setReportVariable("type", QString{ "x" });
        report.dataManager()->setReportVariable("typeX", typeCoords[patient.type].x);
        report.dataManager()->setReportVariable("typeY", typeCoords[patient.type].y);
    }

    fillCommonData(report, patient, doctor, practice);

    report.dataManager()->setReportVariable("ambNum", QString::fromStdString(FreeFn::leadZeroes(amb.number, 12)));

    const char* defaultStatus{ "Не съобщава" };

    report.dataManager()->setReportVariable("allergies",
        patient.allergies.empty() ?
        defaultStatus
        :
        QString::fromStdString(patient.allergies)
    );

    report.dataManager()->setReportVariable("currentDiseases",
                                            patient.currentDiseases.empty() ?
                                            defaultStatus
                                            :
                                            QString::fromStdString(patient.currentDiseases)
    );

    report.dataManager()->setReportVariable("pastDiseases",
        patient.pastDiseases.empty() ?
        defaultStatus
        :
        QString::fromStdString(patient.pastDiseases)
    );


    std::array<bool, 32> temp;

    for (int i = 0; i < 32; i++)
    {
        QString tempVar = "temp" + QString::number(i);
        QString statusVar = "s" + QString::number(i);

        temp[i] = amb.teeth[i].temporary.exists();
        report.dataManager()->setReportVariable(tempVar, temp[i] ? QString{ tempSymbol } : QString{});

        QString status = QString::fromStdString(amb.teeth[i].getStringStatus());

        report.dataManager()->setReportVariable(statusVar, status);

    }


    for (int i = 0; i < 6 && i < selectedProcedures.size(); i++)
    {
        auto& p = selectedProcedures[i];

        QString idx = QString::number(i);

        report.dataManager()->setReportVariable("pDate" + idx , p.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("pDiag" + idx, p.name.c_str());
        report.dataManager()->setReportVariable("pTooth" + idx, QString::number(ToothUtils::getToothNumber(p.tooth, p.temp)));
        report.dataManager()->setReportVariable("pName" + idx, p.name.c_str());
        report.dataManager()->setReportVariable("pNhif" + idx, QString::number(p.code));
        report.dataManager()->setReportVariable("pKsmp" + idx, p.ksmp.c_str());
    }



    auto mdd4idx = dialog.mdd4Referral();

    if (mdd4idx != -1)
    {
        auto& ref = amb.referrals[mdd4idx];
        auto& mddData = std::get<MDD4Data>(ref.data);

        report.dataManager()->setReportVariable("mdd4Num", FreeFn::leadZeroes(ref.number, 12).c_str());
        report.dataManager()->setReportVariable("mdd4Date", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("mdd4Ksmp1", mddData.getKSMP().c_str());
        report.dataManager()->setReportVariable("mdd4Nhif1", mddData.getCode().c_str());
    }

    auto mh119idx = dialog.mh119Referral();

    if (mh119idx != -1)
    {
        auto& ref = amb.referrals[mh119idx];
        auto& data = std::get<MH119Data>(ref.data);

        report.dataManager()->setReportVariable("mh119SpecCode", QString::number(data.specCode));
        report.dataManager()->setReportVariable("mh119Date", ref.date.toBgStandard().c_str());
    }

    //implement the other referrals here:
    report.dataManager()->setReportVariable("refType", QString{ "" });

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

    report.dataManager()->setReportVariable("recipient_name", QString::fromStdString(inv.recipient.name));
    report.dataManager()->setReportVariable("recipient_address", QString::fromStdString(inv.recipient.address));
    report.dataManager()->setReportVariable("recipient_bulstat", QString::fromStdString(inv.recipient.bulstat));
    report.dataManager()->setReportVariable("recipient_phone", QString::fromStdString(inv.recipient.phone));

    report.dataManager()->setReportVariable("issuer_name", QString::fromStdString(inv.issuer.company_name));
    report.dataManager()->setReportVariable("issuer_address", QString::fromStdString(inv.issuer.address_by_contract));
    report.dataManager()->setReportVariable("issuer_bulstat", QString::fromStdString(inv.issuer.bulstat));

    report.dataManager()->setReportVariable("total", formatDoubleWithDecimal(inv.aggragated_amounts.total_amount));

    if (inv.nzokData.has_value())
    {
        report.dataManager()->setReportVariable("practice_rzi", QString::fromStdString(inv.nzokData->rhi_nhif_no));
        report.dataManager()->setReportVariable("contract", QString::fromStdString(inv.nzokData->contract_no + " / " + inv.nzokData->contract_date.toBgStandard()) + " г.");
        report.dataManager()->setReportVariable("mon_notif_number", QString::number(inv.nzokData->fin_document_month_no));
        report.dataManager()->setReportVariable("period", QString::fromStdString("от " + inv.nzokData->date_from.toBgStandard() + " до " + inv.nzokData->date_to.toBgStandard()));

    }

    BusinessOperationModel model{ inv.businessOperations };
    report.dataManager()->addModel("operations", &model, false);

    report.dataManager()->setReportVariable("taxEventDate", QString::fromStdString(inv.aggragated_amounts.taxEventDate.toBgStandard()));
    report.dataManager()->setReportVariable("madeBy", QString::fromStdString(User::doctor().getFullName(false)));
    report.dataManager()->setReportVariable("groundsNoVAT", QString::fromStdString(inv.issuer.grounds_for_not_charging_VAT));
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



    report.dataManager()->setReportVariable("practice_name", QString::fromStdString(practice.name));
    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address));
    report.dataManager()->setReportVariable("doctor", QString::fromStdString(doctor.getFullName()));

    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address));
    report.dataManager()->setReportVariable("declarator", patient.sex ? "Долуподписаната" : "Долуподписаният");
    report.dataManager()->setReportVariable("type", patient.type == 1 ? "ЕГН" : "ЛНЧ");
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
    report.dataManager()->setReportVariable("practice_address", QString::fromStdString(practice.practice_address));
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

void Print::referral(const Referral& ref, const Patient& patient, int ambSheetNumber)
{

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto refCommonFill = [&] {
        report.dataManager()->setReportVariable("ambNum", QString::fromStdString(FreeFn::leadZeroes(ambSheetNumber, 12)));
        report.dataManager()->setReportVariable("refNum", QString::fromStdString(FreeFn::leadZeroes(ref.number, 11)));
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

        if (patient.type == 2) {
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
        report.dataManager()->setReportVariable("specCode", data.specCode);
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
