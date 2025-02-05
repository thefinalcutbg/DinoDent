#include "Printer/Print.h"

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

namespace PrintPrv {

    bool printLogic(LimeReport::ReportEngine& report, const std::string& filepath);
    void fillCommonData(LimeReport::ReportEngine& report, const Patient& patient, const Doctor& doctor, const Practice& practice);
    void fillOutReferrals(const AmbList& amb, LimeReport::ReportEngine& report);
}