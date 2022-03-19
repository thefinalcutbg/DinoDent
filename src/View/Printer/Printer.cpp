#include "Printer.h"
#include <lrreportengine.h>
#include <QString>
#include <QApplication>
#include "Model/CityCode.h"
#include "ProcedurePrintModel.h"
#include "ProcedurePrintSelectDialog.h"
#include "View/ProcedureDisplayModel/ProcedureTableModel.h"
#include "Model/FreeFunctions.h"
#include "qitemselectionmodel.h"
//used as coordinates for the x-es in the checkboxes
struct coords { int x; int y; };
constexpr coords typeCoords[5]{ {0, 0}, { 50, 213 }, { 225, 213 }, {50, 255}, {225, 255} };
constexpr QChar tempSymbol{ 0x25EF };

void Print::ambList(const AmbList& amb, const Patient& patient, const User& user)
{
    std::vector<int> selectedProcedures;

    {
        ProcedurePrintSelectDialog dialog(amb.procedures);

        if (dialog.exec() == QDialog::Rejected) {
            return;
        }

        selectedProcedures = dialog.selectedProcedures();

    }
    
    ProcedurePrintModel model(amb.procedures, selectedProcedures);

    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    report.loadFromFile(":/reports/report_amb.lrxml");
    
    auto& practice = user.practice;
    auto& doctor = user.doctor;
	
	report.dataManager()->setReportVariable("id", QString::fromStdString(patient.id));

    if (patient.type < 5);
    {
        report.dataManager()->setReportVariable("type", QString{ "x" });
        report.dataManager()->setReportVariable("typeX", typeCoords[patient.type].x);
        report.dataManager()->setReportVariable("typeY", typeCoords[patient.type].y);
    }

    report.dataManager()->setReportVariable("city", QString::fromStdString(patient.city));
    report.dataManager()->setReportVariable("address", QString::fromStdString(patient.address));
    report.dataManager()->setReportVariable("patientName", QString::fromStdString(patient.fullName()));
    report.dataManager()->setReportVariable("hirbNo", QString::fromStdString(patient.HIRBNo));
    report.dataManager()->setReportVariable("ambNum", QString::fromStdString(leadZeroes(amb.number, 6)));
    
    
    auto[RHIF, health] = CityCode::getCodes(patient.city);

    report.dataManager()->setReportVariable("RHIFCode", QString::fromStdString(RHIF));
    report.dataManager()->setReportVariable("healthRegion", QString::fromStdString(health));
    report.dataManager()->setReportVariable("birth", QString::fromStdString(patient.birth.toString()));

    report.dataManager()->setReportVariable("RZICode", QString::fromStdString(practice.rziCode));
    report.dataManager()->setReportVariable("specialty", doctor.specialty);
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

        QString status = QString::fromStdString(amb.teeth[i].getSimpleStatus());

        report.dataManager()->setReportVariable(statusVar, status);

    }


    report.dataManager()->addModel("procedures", &model, false);


    QApplication::restoreOverrideCursor();

    report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
    report.setPreviewPageBackgroundColor(QColor(Qt::white));
    
    report.previewReport(LimeReport::PreviewHint::HidePreviewStatusBar);
}

void Print::ambList(const User& user)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();

    auto& doctor = user.doctor;
    
    report.loadFromFile(":/reports/report_amb.lrxml");

    report.dataManager()->setReportVariable("specialty", doctor.specialty);
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(doctor.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(doctor.getFullName(true)));

    ProcedurePrintModel model;

    report.dataManager()->addModel("procedures", &model, false);
    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();

    report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
    report.setPreviewPageBackgroundColor(QColor(Qt::white));
    report.designReport();

}
