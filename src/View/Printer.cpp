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

//used as coordinates for the x-es in the checkboxes
struct coords { int x; int y; };
constexpr coords typeCoords[5]{ {0, 0}, { 50, 213 }, { 225, 213 }, {50, 255}, {225, 255} };
constexpr QChar tempSymbol{ 0x25EF };

void Print::ambList(const AmbList& amb, const Patient& patient)
{
    std::vector<Procedure> selectedProcedures;

    {
        ProcedurePrintSelectDialog dialog(amb.procedures.list());
        
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

    }
    
    ProcedureTableModel model;
    model.setProcedures(selectedProcedures);

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    report.loadFromFile(":/reports/report_amb.lrxml");
    
    auto& practice = User::practice();
    auto& doctor = User::doctor();
	
	report.dataManager()->setReportVariable("id", QString::fromStdString(patient.id));

    if (patient.type < 5);
    {
        report.dataManager()->setReportVariable("type", QString{ "x" });
        report.dataManager()->setReportVariable("typeX", typeCoords[patient.type].x);
        report.dataManager()->setReportVariable("typeY", typeCoords[patient.type].y);
    }

    report.dataManager()->setReportVariable("city", QString::fromStdString(patient.city.getString()));
    report.dataManager()->setReportVariable("address", QString::fromStdString(patient.address));
    report.dataManager()->setReportVariable("patientName", QString::fromStdString(patient.fullName()));
    report.dataManager()->setReportVariable("hirbNo", QString::fromStdString(patient.HIRBNo));
    report.dataManager()->setReportVariable("ambNum", QString::fromStdString(FreeFn::leadZeroes(amb.number, 12)));
    

    report.dataManager()->setReportVariable("RHIFCode", QString::fromStdString(patient.city.getRhif()));
    report.dataManager()->setReportVariable("healthRegion", QString::fromStdString(patient.city.getHealthRegion()));
    report.dataManager()->setReportVariable("birth", QString::fromStdString(patient.birth.toBgStandard()));

    report.dataManager()->setReportVariable("RZICode", QString::fromStdString(practice.rziCode));
    report.dataManager()->setReportVariable("specialty", doctor.specialtyAsInt());
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(doctor.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(doctor.getFullName(true)));

    const char* defaultStatus{ u8"Не съобщатва" };

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


    report.dataManager()->addModel("procedures", &model, false);


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
    report.dataManager()->setReportVariable("number_date", QString::fromStdString(u8" № " + inv.getInvoiceNumber() + u8" от дата " + inv.date.toBgStandard(true)));
    report.dataManager()->setReportVariable("invoice_type", original ? u8"ОРИГИНАЛ" : u8"КОПИЕ");

    auto mainDoc = inv.mainDocument();

    if (mainDoc) {
        report.dataManager()->setReportVariable(
            "main_document",
            QString::fromStdString(
                u8"към фактура № "
                + FreeFn::leadZeroes(mainDoc->number, 10)
                + u8" от " + mainDoc->date.toBgStandard()
                + u8"г."));
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
        report.dataManager()->setReportVariable("contract", QString::fromStdString(inv.nzokData->contract_no + " / " + inv.nzokData->contract_date.toBgStandard()) + u8" г.");
        report.dataManager()->setReportVariable("mon_notif_number", QString::number(inv.nzokData->fin_document_month_no));
        report.dataManager()->setReportVariable("period", QString::fromStdString(u8"от " + inv.nzokData->date_from.toBgStandard() + u8" до " + inv.nzokData->date_to.toBgStandard()));

    }

    BusinessOperationModel model{ inv.businessOperations };
    report.dataManager()->addModel("operations", &model, false);

    report.dataManager()->setReportVariable("taxEventDate", QString::fromStdString(inv.aggragated_amounts.taxEventDate.toBgStandard()));
    report.dataManager()->setReportVariable("madeBy", QString::fromStdString(User::doctor().getFullName(false)));
    report.dataManager()->setReportVariable("groundsNoVAT", QString::fromStdString(inv.issuer.grounds_for_not_charging_VAT));
    report.dataManager()->setReportVariable("paymentType", QString::fromStdString(
        inv.aggragated_amounts.paymentType == PaymentType::Bank ? 
        u8"Банков път" 
        : 
        u8"В брой"
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
    report.dataManager()->setReportVariable("declarator", patient.sex ? u8"Долуподписаната" : u8"Долуподписаният");
    report.dataManager()->setReportVariable("type", patient.type == 1 ? u8"ЕГН" : u8"ЛНЧ");
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
    report.dataManager()->setReportVariable("declarator", patient.sex ? u8"Долуподписаната" : u8"Долуподписаният");
    report.dataManager()->setReportVariable("type", patient.type == 1 ? u8"ЕГН" : u8"ЛНЧ");
    report.dataManager()->setReportVariable("name", QString::fromStdString(patient.fullName()));
    report.dataManager()->setReportVariable("id", QString::fromStdString(patient.id));
    report.dataManager()->setReportVariable("address", QString::fromStdString(patient.getFullAddress()));
    report.dataManager()->setReportVariable("acquainted", patient.sex ? u8"Запозната" : u8"Запознат");
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

    ProcedureTableModel model;

    report.dataManager()->addModel("procedures", &model, false);
    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();

  //  report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
  //  report.setPreviewPageBackgroundColor(QColor(Qt::white));
  //  report.previewReport(LimeReport::PreviewHint::HidePreviewStatusBar);

    report.printReport();

}
