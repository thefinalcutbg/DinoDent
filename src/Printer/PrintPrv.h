#include "Print.h"

#include <QtGlobal>
#include <QProcess>
#ifdef Q_OS_WIN //in MSVC build limereport is dynamic lib
#include <LimeReport/include/lrreportengine.h>
#else
#include "lrreportengine.h"
#endif

#include <QApplication>
#include <QString>
#include <QFileInfo>

#include "View/Widgets/ProcedurePrintSelectDialog.h"
#include "View/TableModels/BusinessOperationModel.h"
#include "View/GlobalFunctions.h"
#include "View/Widgets/InvoicePrintDialog.h"
#include "View/TableModels/FiscalReceiptTableModel.h"
#include "View/TableModels/PlainTableModel.h"
#include "View/ModalDialogBuilder.h"

#include "Model/User.h"
#include "Model/FreeFunctions.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/Financial/Invoice.h"
#include "Model/Referrals/Referral.h"
#include "Model/Dental/SupernumeralPrint.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/NhifSpecReport.h"
#include "Model/Prescription/Prescription.h"
#include "GlobalSettings.h"

#include "Presenter/PatientDialogPresenter.h"

bool inline printLogic(LimeReport::ReportEngine& report, const std::string& filepath)
{
    if (filepath.empty()) {
        return report.printReport();
    }

    return report.printToPDF(filepath.c_str());
}

void inline fillCommonData(LimeReport::ReportEngine& report, const Patient& patient, const Doctor& doctor, const Practice& practice)
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
