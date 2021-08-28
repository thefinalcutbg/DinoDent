#include "Printer.h"
#include <lrreportengine.h>
#include <QString>
#include <QApplication>
#include "Model/CityCode.h"
#include "ProcedurePrintModel.h"

//used as coordinates for the x-es in the checkboxes
struct coords { int x; int y; };
constexpr coords typeCoords[5]{ {0, 0}, { 50, 213 }, { 225, 213 }, {50, 255}, {225, 255} };
constexpr QChar tempSymbol{ 0x25EF };

//placing lead zeroes according to total lenght
QString leadZeroes(int num, int totalLength)
{
    QString formated;
    formated.reserve(totalLength);

    QString number(QString::number(num));

    for (int i = 0; i < totalLength - number.length(); i++)
        formated += "0";

    formated += number;

    return formated;
}

void Print::ambList(const AmbList& amb, const Patient& patient, const User& user)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    report.loadFromFile("reports/amb.lrxml");
    
	
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
    report.dataManager()->setReportVariable("ambNum", leadZeroes(amb.number, 6));

    CityCode code;
    auto[RHIF, health] = code.getCodes(patient.city);

    report.dataManager()->setReportVariable("RHIFCode", QString::fromStdString(RHIF));
    report.dataManager()->setReportVariable("healthRegion", QString::fromStdString(health));
    report.dataManager()->setReportVariable("birth", QString::fromStdString(Date::toString(patient.birth)));

    report.dataManager()->setReportVariable("specialty", user.specialty);
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(user.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(user.getName()));

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

    

    ProcedurePrintModel model(amb.procedures, temp);

    report.dataManager()->addModel("procedures", &model, false);

    QApplication::restoreOverrideCursor();

    //report.designReport();
   // 
    report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
    report.setPreviewPageBackgroundColor(QColor(Qt::white));
    report.previewReport(LimeReport::PreviewHint::HidePreviewStatusBar);
    //report.printReport();
   
}

void Print::ambList(const User& user)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);

    auto report = LimeReport::ReportEngine();
    
    report.loadFromFile("reports/amb.lrxml");

    report.dataManager()->setReportVariable("specialty", user.specialty);
    report.dataManager()->setReportVariable("LPK", QString::fromStdString(user.LPK));
    report.dataManager()->setReportVariable("doctorName", QString::fromStdString(user.getName()));

    ProcedurePrintModel model;

    report.dataManager()->addModel("procedures", &model, false);
    report.setShowProgressDialog(true);
    QApplication::restoreOverrideCursor();

    report.setPreviewScaleType(LimeReport::ScaleType::FitWidth);
    report.setPreviewPageBackgroundColor(QColor(Qt::white));
    report.printReport();

}
