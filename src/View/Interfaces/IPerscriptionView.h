#pragma once
#include <string>
#include <vector>

class PerscriptionPresenter;
class Date;
struct Patient;
struct Dispensation;

class IPerscriptionView
{
public:
	virtual void setPatient(const Patient& patient, const Date& currentDate) = 0;
	virtual void setPresenter(PerscriptionPresenter* p) = 0;
	virtual void setMedicationList(const std::vector<std::string>) = 0;
	virtual void setDispensation(const Dispensation& d) = 0;

};