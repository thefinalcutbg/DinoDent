#include "PrintPrv.h"

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
