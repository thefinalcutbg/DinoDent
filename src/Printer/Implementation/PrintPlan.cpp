#include "PrintPrv.h"
#include "Model/PlainTable.h"
#include "View/TableModels/PlainTableModel.h"
#include "Model/Dental/TreatmentPlan.h"
#include "View/GlobalFunctions.h"
#include "Database/DbDoctor.h"
#include <QDateTime>
bool Print::treatmentPlan(const TreatmentPlan& plan, const Patient& patient, const std::string& pdfFilename)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    report.loadFromFile(":/reports/report_plan.lrxml");


    auto& practice = User::practice();
    auto doctor = DbDoctor::getDoctor(plan.LPK);

    if(!doctor.has_value()){
        QApplication::restoreOverrideCursor();
        return false;
    }

    PrintPrv::fillCommonData(report, patient, *doctor, practice);

    report.dataManager()->setReportVariable("date", plan.date.toBgStandard().c_str());
    report.dataManager()->setReportVariable("printDate", QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
    report.dataManager()->setReportVariable("statusLegend", plan.teeth.getPrintLegend().c_str());
    report.dataManager()->setReportVariable("totalPrice", priceRangeToString(plan.getTotalPrice()));

    PlainTable t;

    const char* columns[] = {
        "stage_name",
        "stage_description",
        "name",
        "tooth",
        "diagnosis",
        "notes",
        "price",
        "stage_price",
        "code"
    };

    const char* stageTitles[] = {
        "ПЪРВИ ЕТАП",
        "ВТОРИ ЕТАП",
        "ТРЕТИ ЕТАП",
        "ЧЕТВЪРТИ ЕТАП",
        "ПЕТИ ЕТАП",
        "ШЕСТИ ЕТАП",
        "СЕДМИ ЕТАП",
        "ОСМИ ЕТАП",
        "ДЕВЕТИ ЕТАП",
        "ДЕСЕТИ ЕТАП"
    };

    for(auto& c : columns){
        t.addColumn({c});
    }

    for(auto &tooth : plan.teeth){
        report.dataManager()->setReportVariable("idx" + QString::number(tooth.index()), tooth.toothIndex().getNhifNumenclature().c_str());

        QString status;

        for(auto& s : tooth.getDetailedPrintStatus()){
            status+=s.c_str();
            status+=" ";
        }

        report.dataManager()->setReportVariable("s" + QString::number(tooth.index()), status);
    }

    for(int i = 0; i < plan.stages.size(); i++){

        auto& stage = plan.stages[i];

        bool isConclusion = &stage == &plan.stages.back() && plan.lastStageIsConclusion;

        if(isConclusion){
            report.dataManager()->setReportVariable("conclusion_title", "ЗАКЛЮЧЕНИЕ");
            report.dataManager()->setReportVariable("conclusion_description", stage.notes.c_str());
            continue;
        }

        auto title = stageTitles[i];

        if(stage.plannedProcedures.empty()){
            t.addCell(0, PlainCell{.data=title});
            t.addCell(1, PlainCell{.data=stage.notes});

            for(int y = 2; y < 9; y++){
                t.addCell(y, {});
            }

            continue;
        }

        auto priceRange = stage.getPriceRange();

        QString stagePrice;

        if(priceRange.first || priceRange.second){
            stagePrice = priceRangeToString(stage.getPriceRange());
        }

        for(auto& planned : stage.plannedProcedures){
            t.addCell(0, {title});
            t.addCell(1, {stage.notes});
            t.addCell(2, {planned.getNameText()});

            std::string toothRange;

            if(std::holds_alternative<ToothIndex>(planned.affectedTeeth)){
                toothRange = std::get<ToothIndex>(planned.affectedTeeth).getNhifNumenclature();
            } else if(std::holds_alternative<ConstructionRange>(planned.affectedTeeth)){

                auto [begin, end] = std::get<ConstructionRange>(planned.affectedTeeth);

                toothRange = ToothUtils::getNomenclature(begin, false) + "-" + ToothUtils::getNomenclature(end, false);
            }

            //clearing for dentures, because tooth count depends on the dental technician
            if(planned.code.type() == ProcedureType::Denture ||
              planned.code.type() == ProcedureType::DenturePair
                ){
                toothRange.clear();
            }

            t.addCell(3, {toothRange});
            t.addCell(4, {planned.getDiagnosisText()});
            t.addCell(5, {planned.notes});
            t.addCell(6, {priceRangeToString(planned.priceRange).toStdString()});
            t.addCell(7, {stagePrice.toStdString()});
            t.addCell(8, {planned.code.code()});
        }
    };

    PlainTableModel m;
    m.setTableData(t);

    report.dataManager()->addModel("procedures", &m, false);

    QApplication::restoreOverrideCursor();

    //report.designReport();

    return PrintPrv::printLogic(report, pdfFilename);
}
