#include "PrintPrv.h"


bool Print::prescription(const Prescription& prescr, const Patient& patient, const std::string& filename)
{
    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_prescr.lrxml");

    report.dataManager()->setReportVariable("nrn", prescr.NRN.c_str());
    report.dataManager()->setReportVariable("doctor", User::doctor().getFullName().c_str());
    report.dataManager()->setReportVariable("LPK", User::doctor().LPK.c_str());
    report.dataManager()->setReportVariable("phone", User::doctor().phone.c_str());
    report.dataManager()->setReportVariable("date", prescr.date.toBgStandard().c_str());
    report.dataManager()->setReportVariable("practice", User::practice().name.c_str());

    const char* dispenseArr[] = { "еднократно", "многократно", "неограничено" };

    QString dispensationStr = "За ";
    dispensationStr += dispenseArr[prescr.dispensation.type];
    dispensationStr += " отпускане";

    if (prescr.dispensation.type == Dispensation::Type::MultipleUse) {
        dispensationStr += " (";
        dispensationStr += QString::number(prescr.dispensation.repeats);
        dispensationStr += " пъти)";
    }

    report.dataManager()->setReportVariable("dispensation", dispensationStr);

    PlainTable table;
    table.addColumn(PlainColumn("med"));

    const char* separator = "________________________________________________________________________________";
    table.addCell(0, { separator });

    for (auto& m : prescr.medicationGroup)
    {
        table.addCell(0, { "<b>" + m.name() + " (" + m.quantityParsed() + ") </b>" });

        table.addCell(0, { m.parseFullDosage() });

        table.addCell(0, { separator });

    }

    if (prescr.supplements.size()) {

        table.addCell(0, { "<b>НЕЛЕКАРСТВЕНИ ДОБАВКИ:</b>" });

        table.addCell(0, { prescr.supplements });

        table.addCell(0, { separator });
    }

    PlainTableModel tableModel;
    tableModel.setTableData(table);

    report.dataManager()->addModel("meds", &tableModel, false);

    report.dataManager()->setReportVariable("patient", patient.fullName().c_str());
    report.dataManager()->setReportVariable("age", patient.getAge(prescr.date));
    report.dataManager()->setReportVariable("city", patient.getFullAddress().c_str());

    std::string female;

    if (prescr.isPregnant) {
        report.dataManager()->setReportVariable("pregnant", "Бременна: ДА");
    }

    if (prescr.isBreastFeeding) {
        report.dataManager()->setReportVariable("breastfeeding", "Кърмачка: ДА");
    }

    QApplication::restoreOverrideCursor();

    if (filename.size()) {

        return report.printToPDF(filename.c_str());
    }

    return report.printReport();

}
