#pragma once
#include <string>
#include <vector>

class PrescriptionPresenter;
class Date;
struct Patient;
struct Dispensation;
class IPatientTileInfo;

class IPrescriptionView
{
public:
	virtual IPatientTileInfo* patientTile() = 0;
	virtual void setPresenter(PrescriptionPresenter* p) = 0;
	virtual void setMedicationList(const std::vector<std::string>) = 0;
	virtual void setDispensation(const Dispensation& d) = 0;
	virtual void setSupplements(const std::string& supplements) = 0;
	virtual void setDate(const Date& date) = 0;
	virtual void setNrn(const std::string& nrn) = 0;
	virtual void setMisc(bool isFemale, bool isPregnant, bool isBreastFeeding) = 0;

};