#pragma once

class PerscriptionPresenter;
class Date;
struct Patient;

class IPerscriptionView
{
public:
	virtual void setPatient(const Patient& patient, const Date& currentDate) = 0;
	virtual void setPresenter(PerscriptionPresenter* p) = 0;

};