#include "PrintPrv.h"


bool Print::invoice(const Invoice& inv, const std::string& pdfFilename)
{
    InvoicePrintDialog d;
    d.exec();

    auto result = d.result();

    if (result == InvoicePrintDialog::Canceled) return false;

    bool original = result == InvoicePrintDialog::Original ? true : false;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    auto report = LimeReport::ReportEngine();
    report.loadFromFile(":/reports/report_invoice.lrxml");

    report.dataManager()->setReportVariable("title", QString::fromStdString(inv.title()));
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
    report.dataManager()->setReportVariable("recipient_bulstat", QString::fromStdString(inv.recipient.identifier));
    report.dataManager()->setReportVariable("recipient_phone", QString::fromStdString(inv.recipient.phone));

    if (inv.recipient.hasVat) {
        report.dataManager()->setReportVariable("recipient_vat", QString::fromStdString(inv.recipient.getVat()));
    }

    auto issuer = inv.issuer();

    report.dataManager()->setReportVariable("issuer_name", QString::fromStdString(issuer.company_name));
    report.dataManager()->setReportVariable("issuer_address", QString::fromStdString(issuer.address_by_contract));
    report.dataManager()->setReportVariable("issuer_bulstat", QString::fromStdString(issuer.bulstat));

    auto vat = issuer.vat();
    if (vat) {
        report.dataManager()->setReportVariable("issuer_vat", QString::fromStdString(vat.value()));

    }

    auto amount = inv.amount();

    report.dataManager()->setReportVariable("amount", formatDoubleWithDecimal(amount));
    report.dataManager()->setReportVariable("vatPercent", inv.isVAT ? 20 : 0);
    double vatAmount = inv.isVAT ? amount * 0.2 : 0;
    report.dataManager()->setReportVariable("vatAmount", formatDoubleWithDecimal(vatAmount));
    report.dataManager()->setReportVariable("total", formatDoubleWithDecimal(amount + vatAmount));

    report.dataManager()->setReportVariable("practice_rzi", QString::fromStdString(User::practice().rziCode));

    if (inv.nhifData.has_value() && User::practice().nhif_contract.has_value())
    {
        auto& contract = *User::practice().nhif_contract;

        report.dataManager()->setReportVariable("contract", QString::fromStdString(contract.contract_no + " / " + contract.date.toBgStandard()) + " г.");
        report.dataManager()->setReportVariable("mon_notif_number", QString::number(inv.nhifData->fin_document_month_no));
        report.dataManager()->setReportVariable("period", QString::fromStdString("от " + inv.nhifData->date_from.toBgStandard() + " до " + inv.nhifData->date_to.toBgStandard()));

    }

    BusinessOperationModel model{ inv.businessOperations };
    report.dataManager()->addModel("operations", &model, false);

    const QString paymentArr[] = { "В брой", "Банков път", "ПОС терминал", "Комбинирано" };

    report.dataManager()->setReportVariable("taxEventDate", QString::fromStdString(inv.taxEventDate.toBgStandard()));
    report.dataManager()->setReportVariable("madeBy", QString::fromStdString(User::doctor().getFullName(false)));

    if (!inv.isVAT) {
        report.dataManager()->setReportVariable("groundsNoVAT", QString::fromStdString(issuer.grounds_for_not_charging_VAT));
        report.dataManager()->setReportVariable("groundsLabel", "Основание за неначисляване на ДДС:");
    }
    report.dataManager()->setReportVariable("paymentType", paymentArr[static_cast<int>(inv.paymentType)]);

    report.dataManager()->setReportVariable("bank", issuer.bank.c_str());
    report.dataManager()->setReportVariable("iban", issuer.iban.c_str());
    report.dataManager()->setReportVariable("bic", issuer.bic.c_str());

    QApplication::restoreOverrideCursor();

    return report.printToPDF(pdfFilename.c_str());
}
