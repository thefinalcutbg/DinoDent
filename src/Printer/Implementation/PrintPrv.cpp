#include "PrintPrv.h"

bool PrintPrv::printLogic(LimeReport::ReportEngine& report, const std::string& filepath)
{
    if (filepath.empty()) {
        return report.printReport();
    }

    return report.printToPDF(filepath.c_str());
}

void PrintPrv::fillCommonData(LimeReport::ReportEngine& report, const Patient& patient, const Doctor& doctor, const Practice& practice)
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
    report.dataManager()->setReportVariable("doctorPhone", QString::fromStdString(doctor.phone));
    report.dataManager()->setReportVariable("practiceName", QString::fromStdString(practice.name));

}

void PrintPrv::fillOutReferrals(LimeReport::ReportEngine& report, const AmbList& amb)
{
    const Referral* form3{ nullptr },
        * form3a{ nullptr },
        * mdd4_1{ nullptr },
        * mdd4_2{ nullptr },
        * mh119{ nullptr };

    for (auto& ref : amb.referrals)
    {
        switch (ref.type)
        {
        case ReferralType::Form3: form3 = &ref; break;
        case ReferralType::Form3A: form3a = &ref; break;
        case ReferralType::MH119: mh119 = &ref; break;
        case ReferralType::MDD4: mdd4_1 ? mdd4_2 = &ref : mdd4_1 = &ref; break;
        }
    }

    if (mdd4_1)
    {
        auto& ref = *mdd4_1;
        auto& mddData = std::get<MDD4Data>(ref.data);

        report.dataManager()->setReportVariable("mdd4Num1", ref.nrn.c_str());
        report.dataManager()->setReportVariable("mdd4Date1", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("mdd4Ksmp1", mddData.getKSMP().c_str());
        report.dataManager()->setReportVariable("mdd4Nhif1", mddData.getCode().c_str());
    }

    if (mdd4_2)
    {
        auto& ref = *mdd4_2;
        auto& mddData = std::get<MDD4Data>(ref.data);

        report.dataManager()->setReportVariable("mdd4Num2", ref.nrn.c_str());
        report.dataManager()->setReportVariable("mdd4Date2", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("mdd4Ksmp2", mddData.getKSMP().c_str());
        report.dataManager()->setReportVariable("mdd4Nhif2", mddData.getCode().c_str());
    }


    if (mh119)
    {
        auto& ref = *mh119;

        switch (std::get<MH119Data>(ref.data).specCode)
        {
        case MH119Data::Pediatric:
            report.dataManager()->setReportVariable("mh119SpecCode1", 61);
            report.dataManager()->setReportVariable("mh119Date1", ref.date.toBgStandard().c_str());
            break;
        case MH119Data::Surgery:
            report.dataManager()->setReportVariable("mh119SpecCode2", 62);
            report.dataManager()->setReportVariable("mh119SpecCode3", 68);
            report.dataManager()->setReportVariable("mh119Date2", ref.date.toBgStandard().c_str());
            break;
        }
    }

    if (form3)
    {

        report.dataManager()->setReportVariable("ref3x", "X");

        auto& ref = *form3;

        auto& refData = std::get<R3Data>(ref.data);

        report.dataManager()->setReportVariable("refNum", ref.nrn.c_str());
        report.dataManager()->setReportVariable("refDate", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("refSpecCode", refData.specialty);
        report.dataManager()->setReportVariable("refMkb", ref.diagnosis.main.code().c_str());
    }

    if (form3a)
    {
        report.dataManager()->setReportVariable("ref3Ax", "X");

        auto& ref = *form3a;

        auto& refData = std::get<R3AData>(ref.data);

        report.dataManager()->setReportVariable("refNum", ref.nrn.c_str());
        report.dataManager()->setReportVariable("refDate", ref.date.toBgStandard().c_str());
        report.dataManager()->setReportVariable("refSpecCode", refData.nhifSpecialty);
        report.dataManager()->setReportVariable("refMkb", ref.diagnosis.main.code().c_str());
        report.dataManager()->setReportVariable("refKSMP", refData.ksmp);
        report.dataManager()->setReportVariable("refHSA", refData.highlySpecializedActivity);
    }
}
